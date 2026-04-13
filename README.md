# STM32-FreeRTOS-Based-Environmental-Monitoring-System-with-TFT-LCD-Dashboard  

This repository contains the source code and documentation for **STM32 FreeRTOS-Based Real-Time Environmental Monitoring System with TFT LCD Dashboard**, where an STM32 microcontroller interfaces with multiple sensors (DHT11, BMP280, MQ135) and RTC to acquire environmental data and display it on an ILI9488 TFT LCD.  
The project demonstrates real-time embedded system design using custom device drivers, FreeRTOS task scheduling, and optimized LCD memory management for smooth, flicker-free rendering.

## 📸 Project Setup

![Environmental Monitoring Setup](./Project%20Photograph.jpeg)


## Features  
- Multi-Sensor Integration: Temperature, humidity, pressure, and air quality monitoring  
- RTC Timekeeping for real-time clock display  
- ILI9488 TFT LCD with optimized memory handling and partial updates  
- Flicker-free UI using change detection  
- FreeRTOS task-based architecture  
- Mutex-based safe data sharing  
- MQ135 signal averaging for noise reduction  


## Components Used  
- STM32F410RB Nucleo Board  
- DHT11 Sensor  
- BMP280 Pressure Sensor  
- MQ135 Gas Sensor  
- ILI9488 TFT LCD (SPI)  


## Tools Used  
- STM32CubeIDE  
- STM32CubeMX  
- FreeRTOS (CMSIS-RTOS)  
- STM32 HAL Drivers  


## Project Structure  
- CODE/BSP → Sensor + LCD drivers   
- CODE/Middleware → RTOS + system logic  
- CODE/Src & CODE/Inc → Application tasks and main logic  
