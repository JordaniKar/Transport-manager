cmake_minimum_required(VERSION 3.15)
project(CityTransport)

# Задаваме модерен C++ стандарт
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Търсим системната библиотека за нишки (заради std::thread)
find_package(Threads REQUIRED)

# Описваме всички сорс файлове (включително и C файла на SQLite)
set(SOURCES
    main.cpp
    BusStop.cpp
    TransitLine.cpp
    TransportManager.cpp
    DatabaseManager.cpp
    sqlite3.c
)

# Създаваме изпълнимия файл
add_executable(${PROJECT_NAME} ${SOURCES})

# Свързваме системните библиотеки към проекта
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

# Тъй като използваш Winsock2, трябва да свържем мрежовата библиотека на Windows
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
endif()