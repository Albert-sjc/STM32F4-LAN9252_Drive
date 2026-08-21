param(
    [Parameter(Mandatory = $true)]
    [string]$Compiler
)

$ErrorActionPreference = 'Stop'

$repositoryRoot = Split-Path -Parent $PSScriptRoot
$compilerDirectory = Split-Path -Parent $Compiler
$symbolReader = Join-Path $compilerDirectory 'arm-none-eabi-nm.exe'
$objectPath = Join-Path ([System.IO.Path]::GetTempPath()) (
    'stm32_pdo_application_' + [System.Guid]::NewGuid().ToString('N') + '.o')

try {
    $compileArguments = @(
        '-std=gnu11',
        '-Wall',
        '-Wextra',
        '-Wno-cpp',
        '-Wno-unused-parameter',
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
        '-c',
        'SSC/Src/SSC-Device.c',
        '-o',
        $objectPath
    )

    Push-Location $repositoryRoot
    try {
        & $Compiler @compileArguments
        if ($LASTEXITCODE -ne 0) {
            throw "SSC-Device.c compilation failed with exit code $LASTEXITCODE"
        }
    }
    finally {
        Pop-Location
    }

    $undefinedSymbols = & $symbolReader --undefined-only $objectPath
    if ($LASTEXITCODE -ne 0) {
        throw "symbol inspection failed with exit code $LASTEXITCODE"
    }

    foreach ($requiredSymbol in @('HAL_GPIO_ReadPin', 'HAL_GPIO_WritePin')) {
        $matchingSymbol = $undefinedSymbols -match (
            "\b$([regex]::Escape($requiredSymbol))$")
        if (-not $matchingSymbol) {
            throw "PDO application does not reference required GPIO operation: $requiredSymbol"
        }
    }

    Write-Output 'PASS: PDO application reads keys and writes LEDs through STM32 HAL.'
}
finally {
    Remove-Item -LiteralPath $objectPath -Force -ErrorAction SilentlyContinue
}
