# CD++ - Extended States

An extended and improved version of CD++ for Cell-DEVS.

## Getting Started

### Download

The easiest way to get the source code is by cloning the git repository:

```
 $ git clone https://github.com/SimulationEverywhere/CDPP_ExtendedStates-codename-Santi.git 
```

### Prerequisites

 * `GCC 4.7` or higher (with C++11 support)
 * `bison`
 * `bison++`
 * `GNU make`

### Compilation

In order to compile the simulator kernel, go to the directory where its source
code is located and run make. For example,

```
 $ cd cdpp/src
 $ make
```
 
Upon finishing, the bin subdirectory will contain the executable file cd++. A
static library containing core functionality of the simulator is also provided
by default.

### Examples

#### Simulation of a Cell-DEVS model

If you wish to simulate the model described by, say, file model.ma,
execute:

```
 $ bin/cd++ -mmodel.ma
```

#### Simulation of an ad-hoc atomic model

For this we suggest to use as baseline the pulse example included in the
project (see `examples/pulse/src`). In particular, it is important to include a
Makefile defining, at least,

 * The `KERNEL_SRC` variable pointing to the directory where the cd++ source
   code is located.
 * The `MODEL_FLAGS` variable with value `-DREGISTER_ATOMICS`. This is to
   compile the code responsible for registering the desired atomic model into
   the simulator kernel.
 * The inclusion of definitions and compilation rules provided by the
   simulator:
```
    include $(KERNEL_SRC)/Makefile.rules
    include $(KERNEL_SRC)/Makefile.defs
```
   
It is also important to include an implementation of the function 

```C++
void register_atomics_on(ParallelModelAdmin &admin)
```

whose purpose is to register into the kernel the atomic models that the user
wants to use. In general, it is sufficient to include a line like the
following:

```C++
admin.registerAtomic(NewAtomicFunction<ModelClass>(), "Model name");
```

Please see the aforementioned example included in the source code for further
details.

## Authors

Updated since 2017 by [Discrete Event Simulation Lab](https://modsimu.exp.dc.uba.ar/sed/), Facultad de Ciencias Exactas y Naturales, Universidad de Buenos Aires (see [changelog](CHANGELOG.md)).

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
