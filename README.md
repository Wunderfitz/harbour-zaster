# ZasterBanker
A banking client for Sailfish OS

## Author
Sebastian J. Wolf [sebastian@ygriega.de](mailto:sebastian@ygriega.de)

## License
Licensed under GNU GPLv3

## Build
Simply clone this repository and use the project file `harbour-zaster.pro` to import the sources in your SailfishOS IDE. To build and run ZasterBanker or an application which is based on ZasterBanker, you need to create the file `harbour-zaster/src/fints/fintsglobals.h` and enter the required constants in the following format:
```
#ifndef FINTSGLOBALS_H
#define FINTSGLOBALS_H
const char FINTS_PRODUCT_ID[] = "1234567890ABCDEF";
const char FINTS_PRODUCT_VERSION[] = "23";
const char FINTS_DUMMY_USER_ID[] = "123456789";
const int SETTINGS_DEFAULT_ENCRYPTION_KEY = 123456789;
#endif // FINTSGLOBALS_H
```

You get the FinTS product ID as soon as you've registered your own application on [hbci-zka.de](https://www.hbci-zka.de/register/prod_register.htm). The default encryption key is only used in case ZasterBanker is unable to determine a unique encryption key from the user's device. Under normal circumstances, ZasterBanker uses an encryption key which was generated automatically. This key is used to encrypt the user's account metadata (not the PIN/password!) on the user's device. Please use a password generator to generate the default key for your application.
