#pragma once

/// <summary>
/// This identifier should be defined before including any of the networking-related header files.
/// It indicates which version of the Wi-Fi data structures the application uses.
/// </summary>
#define NETWORKING_STRUCTS_VERSION 1

/// <summary>
/// This identifier must be defined before including any of the Wi-Fi related header files.
/// It indicates which version of the Wi-Fi data structures the application uses.
/// </summary>
#define WIFICONFIG_STRUCTS_VERSION 1

/// <summary>
/// This identifier must be defined before including any of the UART-related header files.
/// It indicates which version of the UART data structures the application uses.
/// </summary>
#define UART_STRUCTS_VERSION 1

/// <summary>
/// This identifier must be defined before including any of the SPI-related header files.
/// It indicates which version of the SPI data structures the application uses.
/// </summary>
#define SPI_STRUCTS_VERSION 1

//#Defines required for proper Driver Configration
// Defines EPD Display connection Slot 
#define CLICK_SLOT 1

// Defines Which EPD are you using
#define EPD_2in9_H 1  // using 2.9" EPD with IL3820 Driver IC
//#defin