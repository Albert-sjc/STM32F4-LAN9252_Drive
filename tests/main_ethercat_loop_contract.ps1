param(
    [Parameter(Mandatory = $true)]
    [string]$Compiler
)

$ErrorActionPreference = 'Stop'

$repositoryRoot = Split-Path -Parent $PSScriptRoot
$compilerDirectory = Split-Path -Parent $Compiler
$symbolReader = Join-Path $compilerDirectory 'arm-none-eabi-nm.exe'
$objectPath = Join-Path ([System.IO.Path]::GetTempPath()) (
    'main_ethercat_loop_' + [System.Guid]::NewGuid().ToString('N') + '.o')

try {
    $compileArguments = @(
        '-std=gnu11',
        '-Wall',
        '-Wextra',
        '-Werror',
        '-mcpu=cortex-m4',
        '-mthumb',
        '-DSTM32F407xx',
        '-DUSE_HAL_DRIVER',
        '-ICore/Inc',
        '-IDrivers/STM32F4xx_HAL_Driver/Inc',
        '-IDrivers/STM32F4xx_HAL_Driver/Inc/Legacy',
        '-IDrivers/CMSIS/Device/ST/STM32F4xx/Include',
        '-IDrivers/CMSIS/Include',
        '-ISSC/Src',
        '-IEtherCAT',
        '-IEtherCAT/SPIDriver',
        '-IEtherCAT/SPIDriver/STM32F407',
        '-c',
        'Core/Src/main.c',
        '-o',
        $objectPath
    )

    Push-Location $repositoryRoot
    try {
        & $Compiler @compileArguments
        if ($LASTEXITCODE -ne 0) {
            throw "main.c compilation failed with exit code $LASTEXITCODE"
        }
    }
    finally {
        Pop-Location
    }

    $undefinedSymbols = & $symbolReader --undefined-only $objectPath
    if ($LASTEXITCODE -ne 0) {
        throw "symbol inspection failed with exit code $LASTEXITCODE"
    }

    $requiredSymbols = @(
        'LAN9252_Init',
        'MainInit',
        'MainLoop',
        'bRunApplication',
        'HW_Release'
    )

    foreach ($requiredSymbol in $requiredSymbols) {
        $matchingSymbol = $undefinedSymbols -match (
            "\b$([regex]::Escape($requiredSymbol))$")
        if (-not $matchingSymbol) {
            throw "main.c does not reference required EtherCAT symbol: $requiredSymbol"
        }
    }

    Write-Output 'PASS: main.c ARM object contains the complete EtherCAT lifecycle.'
}
finally {
    Remove-Item -LiteralPath $objectPath -Force -ErrorAction SilentlyContinue
}
