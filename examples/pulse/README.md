This is a very basic example of an atomic model that outputs a 3-uple at regular intervals. 

If you take a look at `reg.cpp`, you will see how the model is registered into the simulator kernel:
```
    admin.registerAtomic(NewAtomicFunction<Pulse>(), ATOMIC_MODEL_NAME);
```
Here, `Pulse` is the class name that implements our model and the macro `ATOMIC_MODEL_NAME` defines its name (which has to be unique).

Via `make` we can compile the model:

```
$ make
```

If everything went OK, we should now see the simulator inside the `bin` subdirectory:

```
$ ls bin
cd++  libsimu.a  pulse.o  reg.o
```

Now we can run our simulation using the description provided in the `model` subdirectory:

```
$ bin/cd++ -m../model/pulse.ma -e../model/pulse.ev -ooutput
```

The event file (`pulse.ev`) contains a termination signal sent to one of the input ports of the model. You can see the output tuples by accessing the `output` file:

```
$ head output
00:00:01:000:0 out_port [36, 0, 1]
00:00:02:000:0 out_port [38, 0, 1]
00:00:03:000:0 out_port [21, 0, 1]
00:00:04:000:0 out_port [40, 0, 1]
00:00:05:000:0 out_port [54, 0, 1]
00:00:06:000:0 out_port [46, 0, 1]
00:00:07:000:0 out_port [20, 0, 1]
00:00:08:000:0 out_port [44, 0, 1]
00:00:09:000:0 out_port [86, 0, 1]
00:00:10:000:0 out_port [6, 0, 1]
```

### Data analysis via Python notebooks

The following instructions can be used as a guideline to process and analyze simulation data through Python notebooks:

1. Install [Jupyter Notebook](https://jupyter.org/)
   * You can follow [these steps](https://jupyter.org/install) 
2. Run the `pulse` example as instructed above:
   * `$ bin/cd++ -m../model/pulse.ma -e../model/pulse.ev -ooutput`
   * Make sure your working directory is `src` before doing this
3. Run Jupyter:
   * `$ cd ..`
   * `$ jupyter notebook`
4. Once the web browser opens, go to the `notebook` directory and double-click on `Pulse.ipynb`
5. Click on `Cell -> Run All` in order to run the notebook code and produce a sample graph using the output data
