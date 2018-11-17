/**
 * moved the content from here to .h file since I was getting
 * redefinition error for Wallet::empty function.
 *
 * Once we are done with the code, we will declare the class fully in .h
 * and implement the methods here, for now writing everything in .h works xD
 *
 * If you know how to deal with this please fix
 *
 */
#include <wallet.h>

number Wallet::counter = 21000000 * UNITS_IN_B;
