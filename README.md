# tacOS [![Build Status](https://travis-ci.com/A-Julien/tacOS.svg?branch=master)](https://travis-ci.com/A-Julien/tacOS)

Sauce blanche algérienne chef !

## Project architecture:

*	Github : https://github.com/A-Julien/tacOS
*	CI (Travis) : https://travis-ci.com/A-Julien/tacOS
*	Compte-Rendu  : ```CR.pdf```
*	doc : https://a-julien.github.io/tacOS/

## Testing
## How
All tests are perform by the CI

## Test Architecture

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
|___ ...
```



