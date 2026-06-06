# GERADOR_CKP_CMP

Projeto PlatformIO para Arduino Nano baseado no framework Arduino.

## Ambiente

- Plataforma: `atmelavr`
- Placa: `nanoatmega328`
- Framework: `arduino`
- Serial monitor: `9600`

## Comandos uteis

```powershell
pio run
pio run --target upload
pio device monitor
```

Se o Nano usar o bootloader novo, altere a placa em `platformio.ini` para:

```ini
board = nanoatmega328new
```
