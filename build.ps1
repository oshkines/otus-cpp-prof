# build.ps1 - Скрипт сборки Docker и публикации на GitHub с документацией

Write-Host "============================================================" -ForegroundColor Cyan
Write-Host "     SBORKA PROEKTA V DOCKER I PUBLIKACIA NA GITHUB" -ForegroundColor Cyan
Write-Host "============================================================" -ForegroundColor Cyan
Write-Host ""

# 1. Генерация документации через Doxygen
Write-Host "[0/10] Generacia dokumentacii Doxygen..." -ForegroundColor Yellow
doxygen Doxyfile

if ($LASTEXITCODE -ne 0) {
    Write-Host "Oshibka pri generacii dokumentacii!" -ForegroundColor Red
    exit 1
}
Write-Host "Dokumentacia uspeshno sgenerirovana" -ForegroundColor Green

# 2. Сборка Docker образа
Write-Host "[1/10] Sbor Docker obraza..." -ForegroundColor Yellow
docker build -t cpp-builder .

if ($LASTEXITCODE -ne 0) {
    Write-Host "Oshibka pri sborke Docker obraza!" -ForegroundColor Red
    exit 1
}
Write-Host "Docker obraz sobran" -ForegroundColor Green

# 3. Создание временного контейнера
Write-Host "[2/10] Sozdanie vremennogo konteinera..." -ForegroundColor Yellow
docker create --name temp-container cpp-builder | Out-Null

if ($LASTEXITCODE -ne 0) {
    Write-Host "Oshibka pri sozdanii konteinera!" -ForegroundColor Red
    exit 1
}
Write-Host "Vremenny konteiner sozdan" -ForegroundColor Green

# 4. Копирование собранного .deb пакета из контейнера
Write-Host "[3/10] Kopirovanie .deb paketa iz konteinera..." -ForegroundColor Yellow
docker cp temp-container:/output/. .

if ($LASTEXITCODE -ne 0) {
    Write-Host "Oshibka pri kopirovanii failov!" -ForegroundColor Red
    docker rm temp-container | Out-Null
    exit 1
}
Write-Host ".deb paket skopirovan" -ForegroundColor Green

# 5. Удаление временного контейнера
Write-Host "[4/10] Udalenie vremennogo konteinera..." -ForegroundColor Yellow
docker rm temp-container | Out-Null
Write-Host "Vremenny konteiner udalen" -ForegroundColor Green

# 6. Запрос сообщения коммита
Write-Host "[5/10] Podgotovka kommitu..." -ForegroundColor Yellow
$defaultMessage = "Finish Homework N"
Write-Host "Predlagaemy tekst kommitu: $defaultMessage" -ForegroundColor Cyan
$commitMessage = Read-Host "Vvedite soobshenie kommitu (Enter dlya predlozhennogo)"

if ([string]::IsNullOrWhiteSpace($commitMessage)) {
    $commitMessage = $defaultMessage
}
Write-Host "Budet ispolzovano soobshenie: $commitMessage" -ForegroundColor Green

# 7. Добавление файлов в Git
Write-Host "[6/10] Dobavlenie failov v Git..." -ForegroundColor Yellow
git add .

if ($LASTEXITCODE -ne 0) {
    Write-Host "Oshibka pri dobavlenii failov!" -ForegroundColor Red
    exit 1
}
Write-Host "Faili dobavleny" -ForegroundColor Green

# 8. Коммит
Write-Host "[7/10] Sozdanie kommitu..." -ForegroundColor Yellow
git commit -m "$commitMessage"

if ($LASTEXITCODE -ne 0) {
    Write-Host "Oshibka pri sozdanii kommitu!" -ForegroundColor Red
    exit 1
}
Write-Host "Kommit sozdan" -ForegroundColor Green

# 9. Публикация на GitHub
Write-Host "[8/10] Publikacia na GitHub..." -ForegroundColor Yellow
git push origin main

if ($LASTEXITCODE -ne 0) {
    Write-Host "Oshibka pri publikacii!" -ForegroundColor Red
    exit 1
}
Write-Host "Izmenenia otpravleny v repozitorny" -ForegroundColor Green

# 10. Создание тега
Write-Host "[9/10] Sozdanie tega..." -ForegroundColor Yellow
git tag v0.2.0
git push origin v0.2.0

if ($LASTEXITCODE -ne 0) {
    Write-Host "Oshibka pri sozdanii tega!" -ForegroundColor Red
    exit 1
}
Write-Host "Tag v0.2.0 sozdan i otpravlen" -ForegroundColor Green

Write-Host "============================================================" -ForegroundColor Cyan
Write-Host "SBORKA I PUBLIKACIA ZAVERSHENY USpeshno!" -ForegroundColor Green
Write-Host "============================================================" -ForegroundColor Cyan