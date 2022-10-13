# Template for FW-App with STM32 MCUs

## Project Naming
**fw-app-{project}-{pcbversion}-{AppName}**

## Project Structure
The project may follow the structure below.

    ├── Core/                           # STM32CubeMX generated, main & peripheral initialization
    ├── Drivers/                        # STM32CubeMX generated, HAL library drivers
    ├── fw-lib-stm32-CocktailOS/        # Cocktail OS (submodule)
    ├── fw-lib-stm32-{LibName}/         # FW Library (submodule)
    ├── fw-mod-{project}-{ModuleName}/  # Module Library (submodule)
    ├── App/                            # Application specific
        ├── app.c                       # Pin mappings, Module initializations, etc.
        ├── app_conf.h                  # Application-wide configurations
    ├── Project-Name.ioc                # STM32CubeMX related, configuration file
    ├── .mxproject                      # STM32CubeMX related
    ├── .gitignore                      # git ignore file
    └── README.md

The user code places in ***App*** directory, so that project can acheive abstraction from MCU specific codes (HAL library code). Add submodules with prefix *fw-lib-stm32-* and *fw-mod-* for new functions.

# How to use

## Starting with STM32CubeMX code generation
1. Download and rename root folder with project naming.
Delete example files. (ioc file, mxproject file, and Makefile)
2. Open STM32CubeMX. Go to Project Manager tab.
3. In Project Settings, set Project Location as parent folder of Project Template and Project Name as root folder name.
4. In Code Generator, check "Generate peripheral initializtion as a pair of '.c/.h' files per peripheral.

## Starting from existing project
1. Fork and clone project to local repository.
2. Open project.ioc file and generate code.
3. Add include path and source locations on your compiler.
4. Compile project and enjoy.

## Using FW-Library modules
[FW Library](https://github.com/aniai-dev/FW-Library)

Use git submodule to add STM32LIB modules to the project.

```bash
$ git submodule add https://github.com/aniai-dev/fw-lib-stm32-LibraryName
```

## For Makefile users

### Enable auto routing on fw-app, fw-mod, fw-lib

Add following after ***C_INCLUDES*** is defined.
```make
C_INCLUDES = \
*something here* \
*something here*

LIB_SOURCES = $(wildcard **/App/*.c) $(wildcard **/Target/*.c) $(wildcard **/*.c)
C_SOURCES += $(LIB_SOURCES)
LIB_HEADERS = $(wildcard **/App/*.h) $(wildcard **/Target/*.h) $(wildcard **/*.h)
C_INCLUDES += $(addprefix -I, $(sort $(dir $(LIB_HEADERS))))
$(info Makefile compiles $(LIB_SOURCES))
$(info Makefile includes $(LIB_HEADERS))
```
### Enable st-flash on Makefile
Add following to end of Makefile

    flash:
        st-flash write $(BUILD_DIR)/$(TARGET).bin 0x8000000
    
Then, you can flash by

```bash
$ make flash
```
