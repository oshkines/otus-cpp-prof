# build.ps1 - Скрипт сборки Docker и публикации на GitHub

Write-Host "============================================================" -ForegroundColor Cyan
Write-Host "     SBORKA PROEKTA V DOCKER I PUBLIKACIA NA GITHUB" -ForegroundColor Cyan
Write-Host "============================================================" -ForegroundColor Cyan
Write-Host ""

# 1. Сборка Docker образа
Write-Host "[1/6] Sbor Docker obraza..." -ForegroundColor Yellow
docker build -t cpp-builder .

if ($LASTEXITCODE -ne 0) {
    Write-Host "Oshibka pri sborke Docker obraza!" -ForegroundColor Red
    exit 1
}
Write-Host "Docker obraz sobran" -ForegroundColor Green

# 2. Создание временного контейнера
Write-Host "[2/6] Sozdanie vremennogo konteinera..." -ForegroundColor Yellow
docker create --name temp-container cpp-builder | Out-Null

if ($LASTEXITCODE -ne 0) {
    Write-Host "Oshibka pri sozdanii konteinera!" -ForegroundColor Red
    exit 1
}
Write-Host "Vremenny konteiner sozdan" -ForegroundColor Green

# 3. Копирование собранного .deb пакета из контейнера
Write-Host "[3/6] Kopirovanie .deb paketa iz konteinera..." -ForegroundColor Yellow
docker cp temp-container:/output/. .

if ($LASTEXITCODE -ne 0) {
    Write-Host "Oshibka pri kopirovanii failov!" -ForegroundColor Red
    docker rm temp-container | Out-Null
    exit 1
}
Write-Host ".deb paket skopirovan" -ForegroundColor Green

# 4. Удаление временного контейнера
Write-Host "[4/6] Udalenie vremennogo konteinera..." -ForegroundColor Yellow
docker rm temp-container | Out-Null
Write-Host "Vremenny konteiner udalen" -ForegroundColor Green

# 5. Запрос сообщения коммита
Write-Host "[5/6] Podgotovka kommitu..." -ForegroundColor Yellow
$defaultMessage = "Finish Homework N"
Write-Host "Predlagaemy tekst kommitu: $defaultMessage" -ForegroundColor Cyan
$commitMessage = Read-Host "Vvedite soobshenie kommitu (Enter dlya predlozhennogo)"

if ([string]::IsNullOrWhiteSpace($commitMessage)) {
    $commitMessage = $defaultMessage
}
Write-Host "Budet ispolzovano soobshenie: $commitMessage" -ForegroundColor Green

# 6. Добавление файлов в Git
Write-Host "[6/8] Dobavlenie failov v Git..." -ForegroundColor Yellow
git add .

if ($LASTEXITCODE -ne 0) {
    Write-Host "Oshibka pri dobavlenii failov!" -ForegroundColor Red
    exit 1
}
Write-Host "Faili dobavleny" -ForegroundColor Green

# 7. Коммит
Write-Host "[7/8] Sozdanie kommitu..." -ForegroundColor Yellow
git commit -m "$commitMessage"

if ($LASTEXITCODE -ne 0) {
    Write-Host "Oshibka pri sozdanii kommitu!" -ForegroundColor Red
    exit 1
}
Write-Host "Kommit sozdan" -ForegroundColor Green

# 8. Публикация на GitHub
Write-Host "[8/8] Publikacia na GitHub..." -ForegroundColor Yellow
git push origin main

if ($LASTEXITCODE -ne 0) {
    Write-Host "Oshibka pri publikacii!" -ForegroundColor Red
    exit 1
}
Write-Host "Izmenenia otpravleny v repozitorny" -ForegroundColor Green

Write-Host "============================================================" -ForegroundColor Cyan
Write-Host "SBORKA I PUBLIKACIA ZAVERSHENY USpeshno!" -ForegroundColor Green
Write-Host "============================================================" -ForegroundColor Cyan