# Test Unit
All tests are perform by the CI

The ```test_unit.sh```  introspect all ```test_step<X>``` folder and launch tests.
Each test script need to have a ```<testX>_result``` file in the ```results_test``` folder.
If the test script need a input, store the input file in ```input_test``` folder 
(the script check automaticly if their are a input file or not). The input test file 
need to be call like ```<testX>_input```. 

Please make sure that your files architecture repect the Testing 
architecture according the following architecture schematic

## Test Architecture schematic

```
test
│   README.md
|   test_unit.sh
└───test_step2
│   │   test1.c
│   │   test2.c
│   │   ...
│   │   
│   └───input_test
|   │    │   test1_input
|   │    │   test2_input
|   │    │   ...
|   └──results_test
|   │    │   test1_result
|   │    │   test2_input
|   │    │   ...
│   
└───test_step3
|   │   ...
|
└─── ...
```