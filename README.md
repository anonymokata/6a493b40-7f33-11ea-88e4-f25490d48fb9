### Purpose and Scope ###

See the following URLs:
https://kata.pillartechnology.com/
https://github.com/PillarTechnology/kata-checkout-order-total

### Build and Run ###

Build with GCC from a UNIX command shell using the following command:

g++ -std=c++11 Checkout.cpp -o Checkout

If making changes, use the following:

g++ -std=c++11 -Wall -Wextra -pedantic-errors Checkout.cpp -o Checkout

Run as follows:

./Checkout

Suggested build environments:
1)	MacOS with Xcode installed. Use Terminal application for shell.
2)	Windows with Cygwin installed, which in turn has GCC installed.
3)	Linux with GCC installed.

I have only tested option 1), but am relatively sure it will work in the other environments. I did my development on Windows 10, with Visual Studio Community 2019.

### Command Line Interface Commands ###

ci – (Configure Item) Add a new grocery item to the configuration.
cs – (Configure Special) Add a Special to a previously configured grocery item.
lc – (List Configuration) List the current configuration.
clearconfig – (Clear Configuration) Clear the current configuration.
ai – (Add Item) Add an item to the checkout total.
ri – (Remove Item) Remove an item from the checkout total.
clearcheckout – (Clear Checkout) Clear the items in the checkout total.
rt – (Run Tests) Run the embedded tests.
loadtestconfig – (Load Test Configuration) Load the constant configuration data in the test vectors to the main configuration object. This allows a configuration to be setup without having to configure every item from the command line.

### NOTES ###

The convention of truncating (not rounding) unit price fractional pennies was adopted; favoring the customer. Because of this, some results may appear slightly off, if one is expecting rounding.

As indicated by the command line interface prompts, prices are entered in cents and weights in centi-pounds (1 pound = 100 centi-pounds).  
