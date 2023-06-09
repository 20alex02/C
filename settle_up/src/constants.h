#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#ifndef RATING_DECIMALS_OPT
#define RATING_DECIMALS_OPT 4
#endif

#ifndef PAYMENT_DECIMALS_OPT
#define PAYMENT_DECIMALS_OPT 2
#endif

enum constants
{
    RATING_DECIMALS = RATING_DECIMALS_OPT,
    PAYMENT_DECIMALS = PAYMENT_DECIMALS_OPT + 1,
};

#endif
