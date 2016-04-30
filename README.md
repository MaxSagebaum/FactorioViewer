# FactorioViewer

A small project that be used to create graphs for [factorio](https://www.factorio.com/) production lines.

A sample call of the library produces an ascii formatted graph:
~~~
 ./build/FactorioViewer fast-inserter -u 20
fast-inserter        electronic-circuit       iron-plate
  20.000 u/min --+->   40.000 u/min     --+->   40.000 u/min
   0.167 fabs    |      0.333 fabs        |      2.333 fabs
                 |                        |
                 |                        |   copper-cable         copper-plate
                 |                        +->  240.000 u/min ---->  120.000 u/min
                 |                               0.500 fabs           7.000 fabs
                 |
                 |   iron-plate
                 +->   40.000 u/min
                 |      2.333 fabs
                 |
                 |   basic-inserter       electronic-circuit       iron-plate
                 +->   20.000 u/min --+->   20.000 u/min     --+->   20.000 u/min
                        0.167 fabs    |      0.167 fabs        |      1.167 fabs
                                      |                        |
                                      |                        |   copper-cable         copper-plate
                                      |                        +->  120.000 u/min ---->   60.000 u/min
                                      |                               0.250 fabs           3.500 fabs
                                      |
                                      |   iron-gear-wheel       iron-plate
                                      +->   20.000 u/min  ---->   40.000 u/min
                                      |      0.167 fabs            2.333 fabs
                                      |
                                      |   iron-plate
                                      +->   20.000 u/min
                                             1.167 fabs

copper-plate         iron-plate
 180.000 u/min        160.000 u/min
  10.500 fabs           9.333 fabs
~~~

The same graph can also be generated in the dot format and then rendered with graphviz:
~~~
 ./build/FactorioViewer fast-inserter -u 20 -d > fast-inserter.dot
 dot -Tpng fast-inserter.dot > fast-inserter.png
~~~

The path for the factorio recipes can be set with the '--dir' option. The default path is the steam linux directory.
Afterwards the path is stored in an ini files and does not need to be given each time.

In order to compile the library you need to call:

~~~
mkdir build
cd build
cmake ../
make
~~~

The program reads the recipe files from factorio and needs therefore an installed lua library.

For a more detailed help see
~~~
 ./build/FactorioViewer --help
~~~

This is only a first and rough solution of my idea for a tool that can be used easily to get an overview of the required
amount of resources for x items per minute. After I finish my thesis, the code will be cleaned up, refactored and document.
Until then please report any bugs, unusual behaviour or feature requests.

TODO list:
 - Add i18n support from the factorio files.