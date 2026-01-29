<a id="readme-top"></a>


<h1 align="center">HomeSec-esp</h1>

  <p align="center">
    An ESP32 project that currently connects to Wi-Fi. More features are coming soon.
  </p>

<!-- ABOUT THE PROJECT -->
## About The Project

This project is an ESP32-based home security system built using the ESP-IDF framework.

At its current stage, the firmware just connects to Wi-Fi.

The goal of this project is to gradually evolve into a full-featured home security system.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- PREREQUISITES -->
## Prerequisites

 - Python 3.8+ (Required by ESP-IDF)
 - Git
### Install ESP-IDF

Follow the official Espressif installation guide:

https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- HOW TO USE -->
## How to Use

1. Clone the repository:
    ```sh
    git clone https://github.com/ashiqr-dev/homeSec-esp.git
    cd homeSec-esp
    ```
2. Open the project in the VS Code Dev Container (`F1` -> `Reopen in Container`).

3. To edit Wi-Fi settings:
    ```sh
    idf.py menuconfig
    ```

4. Scroll to and open `Wi-Fi Configuration`.

5. Edit Wi-Fi configuration and save it.

6. To build and flash, run 
    ```sh
    idf.py build flash
    ```

7. To monitor serial output:
    ```sh
    idf.py monitor
    ```

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Issues

See the [open issues](https://github.com/ashiqr-dev/homeSec-esp/issues) for a full list of proposed features (and known issues).

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE.txt` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTACT -->
## Contact

Ashiq Renju - [@ashiqr-dev](https://github.com/ashiqr-dev/) - ashiqr.dev@gmail.com

Project Link: [https://github.com/ashiqr-dev/homeSec-esp/](https://github.com/ashiqr-dev/homeSec-esp/)


<p align="right">(<a href="#readme-top">back to top</a>)</p>