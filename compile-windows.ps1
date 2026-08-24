param ([string]$Mode = "")

if ($Mode -eq "--force") {

    if (Test-Path build) {
        Write-Host "Limpando a pasta build antiga..." -ForegroundColor Yellow
        Remove-Item -Recurse -Force build
    }
}

cmake -S . -B build -G Ninja -DCMAKE_CXX_COMPILER="C:/msys64/ucrt64/bin/g++.exe"

cmake --build build
