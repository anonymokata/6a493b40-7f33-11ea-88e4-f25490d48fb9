#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

///////////////////////////////////////////////////////////////////////////////
//                  Common data type definitions                             //
///////////////////////////////////////////////////////////////////////////////

const unsigned int CENTI_POUNDS_PER_POUND = 100;
const unsigned int CENTS_PER_DOLLAR = 100;
const unsigned int QUANTITY_LOW_LIMIT = 1;
const unsigned int QUANTITY_HIGH_LIMIT = 1000;
const unsigned int PRICE_LOW_LIMIT = 1;
const unsigned int PRICE_HIGH_LIMIT = 1000000;

#define INVALID_ENTRY cout << "Invalid entry." << endl
#define PRINT_PASS_END cout << " PASS." << endl
#define PRINT_FAIL_END { cout << " FAIL." << endl << endl << "*** FAIL ***" << endl << endl; overallResult = false; }

typedef enum
{
    ITEM_UNITS_EACH,
    ITEM_UNITS_WEIGHT
} ITEM_UNITS_T;

typedef enum
{
    SPECIAL_MARK_DOWN,
    SPECIAL_BUY_N_GET_M_AT_X_PCT_OFF,
    SPECIAL_N_FOR_X_DOLLARS,
    SPECIAL_BUY_N_GET_M_OF_EQUAL_OR_LESSER_VALUE_FOR_X_PCT_OFF
} SPECIAL_TYPE_T;

typedef struct
{
    SPECIAL_TYPE_T specialType;
    unsigned int buyN;
    unsigned int getM;

    union
    {
        unsigned int pctOff;
        unsigned int priceCentsForN;
    };

} SPECIAL_ATTRIBUTES_T;

typedef struct
{
    unsigned int priceCents;
    ITEM_UNITS_T    units;
    SPECIAL_ATTRIBUTES_T* specialAttributesPtr;
} ITEM_ATTRIBUTES_T;



///////////////////////////////////////////////////////////////////////////////
//                  ItemConfigurationData Class                              //
///////////////////////////////////////////////////////////////////////////////

class ItemConfigurationData
{
public:
    ItemConfigurationData();
    ~ItemConfigurationData();

    bool AddItem( const string& itemName, ITEM_UNITS_T units, unsigned int priceCents );
    bool Contains( const string& itemName );
    bool AddSpecial( const string& itemName, const SPECIAL_ATTRIBUTES_T& specialAttributes );
    ITEM_ATTRIBUTES_T GetItemAttributes( const string& itemName );
    void ClearData( void );
    void DisplayItemConfigurationData( void );
    void DisplayItemConfigurationData( const string& itemName );
    string& GetQuantityString( unsigned int quantity, ITEM_UNITS_T units, string& resultStr );

protected:

private:

    map<string, ITEM_ATTRIBUTES_T>* itemConfigurationDataPtr;

    void DisplayItemConfiguration( map<string, ITEM_ATTRIBUTES_T>::iterator itr );
};


///////////////////////////////////////////////////////////////////////////////
//                        CheckoutTotal Class                                //
///////////////////////////////////////////////////////////////////////////////

class CheckoutTotal
{
public:
    CheckoutTotal();
    ~CheckoutTotal();

    void AddItem( ItemConfigurationData * itemConfigData, const string& itemName, unsigned int quantity );
    bool RemoveItem( ItemConfigurationData * itemConfigData, string itemName, unsigned int quantity );
    unsigned int GetCheckoutTotal( void ) { return checkoutTotal; }
    void ClearData( void );

protected:

private:

    typedef struct
    {
        unsigned int quantity;
        unsigned int groupPrice;
    } ITEM_ATTRIBUTES_CHECKOUT_TOTAL_T;

    unsigned int checkoutTotal;

    map<string, ITEM_ATTRIBUTES_CHECKOUT_TOTAL_T>* checkoutTotalDataPtr;

    bool Contains( const string& itemName );
    unsigned int GetGroupPrice( ItemConfigurationData * itemConfigData, const string& itemName );
    void UpdateCheckoutTotal( void );
    unsigned int Apply_BuyN_GetM_At_X_PctOff( ITEM_ATTRIBUTES_T& itemAttr, unsigned int quantity, unsigned int unitsDivider );
    unsigned int ComputePriceAfterPctOff( unsigned int quantity, unsigned int unitPrice, unsigned int pctOff, unsigned int unitsDivier );
};


///////////////////////////////////////////////////////////////////////////////
//                      CheckoutTotalTest Class                              //
///////////////////////////////////////////////////////////////////////////////

class CheckoutTotalTest
{
public:
    CheckoutTotalTest();
    ~CheckoutTotalTest();

    void RunTests( void );

protected:

private:
    bool overallResult;
    ItemConfigurationData * itemConfigurationData;
    CheckoutTotal * checkoutTotal;

    // vector< tuple< itemName, units, priceCents > >
    vector< tuple <string, ITEM_UNITS_T, unsigned int> > * itemConfigTestDataVector;

    // vector< tuple< itemName, specialType, buyN, getM, pctOff OR priceCentsForN > >
    vector< tuple <string, SPECIAL_TYPE_T, unsigned int, unsigned int, unsigned int> > * itemConfigSpecialTestDataVector;

    // vector< tuple< itemName, add (remove), quantity, checkoutTotal > >
    vector< tuple <string, bool, unsigned int, unsigned int> > * checkoutTotalTestDataVector;

    bool ActualVsExpected( const string& itemName, const string& paramName, unsigned int expected, unsigned int actual );
    void DisplayTestResultsBanner( const string& title );
    void AddAndReadBackTestOfItemConfiguration( void );
    void AddAndReadBackTestOfSpecialConfiguration( void );
    void CheckoutRunningTotalTest( void );
    void InitializeTestDataVectors( void );
};


///////////////////////////////////////////////////////////////////////////////
//                  Common function prototypes                               //
///////////////////////////////////////////////////////////////////////////////

bool ValidateNumericEntryString( const string& inLine, unsigned int * outVal, unsigned int lowLimit, unsigned int upLimit );
bool ReadPriceParam( unsigned int * inVal, const string& inBuf, const string& paramName );
bool ReadQuantityParam( unsigned int * inVal, ITEM_UNITS_T units, const string& paramName, unsigned int lowLimit = QUANTITY_LOW_LIMIT );
void ReadItemConfiguration( ItemConfigurationData * itemConfigurationData );
void ReadSpecialConfiguration( ItemConfigurationData * itemConfigurationData );
void ReadCheckoutItemAddRemove( ItemConfigurationData * itemConfigurationData, CheckoutTotal * checkoutTotal, bool isAdd );
string CentsToDollarsString( unsigned int cents );


///////////////////////////////////////////////////////////////////////////////
//                                 main()                                    //
///////////////////////////////////////////////////////////////////////////////

int main( void )
{
    string inBuf1;

    ItemConfigurationData * itemConfigurationData = new ItemConfigurationData;
    CheckoutTotal * checkoutTotal = new CheckoutTotal;
    CheckoutTotalTest* checkoutTotalTest = new CheckoutTotalTest;

    while ( inBuf1 != "quit" )
    {
        cout << ">";
        getline( std::cin >> std::ws, inBuf1 );

        if ( inBuf1 == "ci" ) // configitem
            ReadItemConfiguration( itemConfigurationData );
        else if ( inBuf1 == "cs" ) // configspecial
            ReadSpecialConfiguration( itemConfigurationData );
        else if ( inBuf1 == "lc" ) // listconfig
            itemConfigurationData->DisplayItemConfigurationData();
        else if ( inBuf1 == "ai" ) // additem
            ReadCheckoutItemAddRemove( itemConfigurationData, checkoutTotal, true );
        else if ( inBuf1 == "ri" ) // removeitem
            ReadCheckoutItemAddRemove( itemConfigurationData, checkoutTotal, false );
        else if ( inBuf1 == "rt" ) // runtests
            checkoutTotalTest->RunTests();
        else if ( inBuf1 == "quit" )
            break;
        else
            INVALID_ENTRY;
    }

    delete itemConfigurationData;
    delete checkoutTotal;
    delete checkoutTotalTest;

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
//                     Command Line Interface                                //
///////////////////////////////////////////////////////////////////////////////

bool ValidateNumericEntryString( const string& inLine, unsigned int * outVal, unsigned int lowLimit, unsigned int upLimit )
{
    bool result = true;

    for ( unsigned int i = 0; i < inLine.size(); i++ )
        if ( !isdigit( inLine [ i ] ) )
        {
            result = false;
            break;
        }

    if ( result )
    {
        ( * outVal ) = stoi( inLine );
        result = ( ( * outVal ) >= lowLimit && ( * outVal ) <= upLimit ) ? true : false;
    }

    return result;
}

bool ReadPriceParam( unsigned int * inVal, const string& inBuf, const string& paramName )
{
    bool result = true;

    cout << "Enter " << paramName << " in cents (" << to_string( PRICE_LOW_LIMIT ) << " - " << to_string( PRICE_HIGH_LIMIT ) << "): ";
    getline( cin >> ws, const_cast< string& >( inBuf ) );

    if ( !ValidateNumericEntryString( inBuf, inVal, PRICE_LOW_LIMIT, PRICE_HIGH_LIMIT ) )
        result = false;

    return result;
}

bool ReadQuantityParam( unsigned int * inVal, ITEM_UNITS_T units, const string& paramName, unsigned int lowLimit )
{
    bool result = true;
    unsigned int upLimit = QUANTITY_HIGH_LIMIT;
    string promptStr, inBuf;

    if ( units == ITEM_UNITS_EACH )
        promptStr = "Enter " + paramName + " quantity (" + to_string( lowLimit ) + " - " + to_string( QUANTITY_HIGH_LIMIT ) + "): ";
    else
    {
        upLimit *= CENTI_POUNDS_PER_POUND;
        promptStr = "Enter " + paramName + " quantity in centi-pounds (" + to_string( lowLimit ) + " - "
            + to_string( upLimit ) + "). For example enter " + to_string( CENTI_POUNDS_PER_POUND ) + " for 1 lb.: ";
    }

    cout << promptStr;
    getline( cin >> ws, inBuf );

    if ( !ValidateNumericEntryString( inBuf, inVal, lowLimit, upLimit ) )
        result = false;

    return result;
}

void ReadItemConfiguration( ItemConfigurationData * itemConfigurationData )
{
    string itemName, inBuf;
    unsigned int tempInt;
    ITEM_UNITS_T units;

    cout << "Enter Item name: ";
    getline( cin >> ws, itemName );

    if ( itemConfigurationData->Contains( itemName ) )
        cout << "Item already exists." << endl;
    else
    {
        cout << "Units are 0) Each, or 1) Weight. Enter 0 or 1: ";
        getline( std::cin >> std::ws, inBuf );

        if ( !ValidateNumericEntryString( inBuf, &tempInt, 0, 1 ) )
            INVALID_ENTRY;
        else
        {
            units = ( ITEM_UNITS_T ) tempInt;

            if ( !ReadPriceParam( &tempInt, inBuf, "priceCents" ) )
                INVALID_ENTRY;
            else
                itemConfigurationData->AddItem( itemName, units, tempInt );
        }
    }
}

// Manages the command line interface for configuring Specials. (Command "cs").
// Prompts the User for required data, validates the data, and displays error messages if appropriate.
void ReadSpecialConfiguration( ItemConfigurationData * itemConfigurationData )
{
    bool allEntriesAreValid = true;
    string itemName, inBuf1;
    unsigned int tempInt;
    SPECIAL_ATTRIBUTES_T specialAttributes;

    memset( &specialAttributes, 0, sizeof( SPECIAL_ATTRIBUTES_T ) );

    cout << "Enter the name of the item the Special will be associated with: ";
    getline( cin >> ws, itemName );

    if ( !itemConfigurationData->Contains( string( itemName ) ) )
    {
        INVALID_ENTRY;
        cout << "Item does not exist in Configuration Data" << endl;
    }
    else
    {
        cout << "Choose Special type: " << endl;
        cout << "0) SPECIAL_MARK_DOWN" << endl;
        cout << "1) SPECIAL_BUY_N_GET_M_AT_X_PCT_OFF" << endl;
        cout << "2) SPECIAL_N_FOR_X_DOLLARS" << endl;
        cout << "3) SPECIAL_BUY_N_GET_M_OF_EQUAL_OR_LESSER_VALUE_FOR_X_PCT_OFF" << endl;
        cout << "Enter 0, 1, 2 or 3: ";

        getline( cin >> ws, inBuf1 );

        if ( !ValidateNumericEntryString( inBuf1, &tempInt, 0, 3 ) )
            INVALID_ENTRY;
        else
        {
            specialAttributes.specialType = ( SPECIAL_TYPE_T ) tempInt;

            if ( specialAttributes.specialType != SPECIAL_MARK_DOWN )
            {
                // Read buyN
                if ( !ReadQuantityParam( &tempInt, itemConfigurationData->GetItemAttributes( string( itemName ) ).units, "buyN" ) )
                    allEntriesAreValid = false;
                else
                    specialAttributes.buyN = tempInt;

                if ( allEntriesAreValid && specialAttributes.specialType != SPECIAL_N_FOR_X_DOLLARS )
                {
                    // Read getM
                    if ( !ReadQuantityParam( &tempInt, itemConfigurationData->GetItemAttributes( string( itemName ) ).units, "getM" ) )
                        allEntriesAreValid = false;
                    // Implements the "lesser of" requirement of SPECIAL_BUY_N_GET_M_OF_EQUAL_OR_LESSER_VALUE_FOR_X_PCT_OFF,
                    // by ensuring M is always less than or equal to N.
                    else if ( allEntriesAreValid && specialAttributes.specialType == SPECIAL_BUY_N_GET_M_OF_EQUAL_OR_LESSER_VALUE_FOR_X_PCT_OFF &&
                              tempInt > specialAttributes.buyN )
                    {
                        cout << endl << "getM must be less than or equal to buyN." << endl;
                        allEntriesAreValid = false;
                    }
                    else if ( allEntriesAreValid )
                        specialAttributes.getM = tempInt;
                }
            }

            if ( allEntriesAreValid && specialAttributes.specialType != SPECIAL_N_FOR_X_DOLLARS )
            {
                cout << "Enter pctOff (1 - 100): ";
                getline( cin >> ws, inBuf1 );
                if ( !ValidateNumericEntryString( inBuf1, &tempInt, 1, 100 ) )
                    allEntriesAreValid = false;
                else
                    specialAttributes.pctOff = tempInt;
            }

            if ( allEntriesAreValid && specialAttributes.specialType == SPECIAL_N_FOR_X_DOLLARS )
            {
                if ( !ReadPriceParam( &tempInt, inBuf1, "priceCentsForN" ) )
                    allEntriesAreValid = false;
                else
                    specialAttributes.priceCentsForN = tempInt;
            }

            if ( !allEntriesAreValid )
                INVALID_ENTRY;
            else if ( !itemConfigurationData->AddSpecial( string( itemName ), specialAttributes ) )
                cout << "Error adding special to Configuration Data" << endl;
        }
    }
}

// Manages the command line interface for adding and removing items to/from the checkout total. (Commands "ai" and "ri").
// Prompts the User for required data, validates the data, and displays error messages if appropriate.
// Displays information about the item added, and the previous and current checkout totals.
void ReadCheckoutItemAddRemove( ItemConfigurationData * itemConfigurationData, CheckoutTotal * checkoutTotal, bool isAdd )
{
    string itemName, tempBuf;
    unsigned int tempInt;
    unsigned int prevCheckoutTotal = checkoutTotal->GetCheckoutTotal();

    cout << "Enter Item name: ";
    getline( cin >> ws, itemName );

    if ( !itemConfigurationData->Contains( itemName ) ) // If the requested item has not been configured.
    {
        cout << itemName << " does not exist in configuration." << endl;
    }
    else
    {
        if ( !ReadQuantityParam( &tempInt, itemConfigurationData->GetItemAttributes( string( itemName ) ).units, "" ) )
            INVALID_ENTRY;  // The entered quntity is not valid.
        else
        {
            if ( isAdd )
                checkoutTotal->AddItem( itemConfigurationData, itemName, tempInt );
            else if ( !checkoutTotal->RemoveItem( itemConfigurationData, itemName, tempInt ) )
                cout << endl << "Not available for removal." << endl;

            // Display the item quantity, followed by the item configuration info, then the previous and current
            // checkout totals.
            cout << endl << itemConfigurationData->GetQuantityString(
                tempInt, itemConfigurationData->GetItemAttributes( string( itemName ) ).units, tempBuf ) << "\t";
            itemConfigurationData->DisplayItemConfigurationData( itemName );
            cout << "Previous checkout total = " << CentsToDollarsString( prevCheckoutTotal ) << endl;
            cout << "New checkout total = " << CentsToDollarsString( checkoutTotal->GetCheckoutTotal() ) << endl;
        }
    }
}

string CentsToDollarsString( unsigned int cents )
{
    string temp = to_string( cents % CENTS_PER_DOLLAR );
    if ( stoi( temp ) < 10 )
        temp.insert( 0, "0" );
    return to_string( cents / CENTS_PER_DOLLAR ) + "." + temp;
}

///////////////////////////////////////////////////////////////////////////////
//                  ItemConfigurationData Class                              //
///////////////////////////////////////////////////////////////////////////////

ItemConfigurationData::ItemConfigurationData()
{
    itemConfigurationDataPtr = new map<string, ITEM_ATTRIBUTES_T>;
}

ItemConfigurationData::~ItemConfigurationData()
{
    ClearData();
    delete itemConfigurationDataPtr;
}

bool ItemConfigurationData::AddItem( const string& itemName, ITEM_UNITS_T units, unsigned int priceCents )
{
    bool result = false;

    if ( !Contains( itemName ) )
    {
        itemConfigurationDataPtr->insert( pair<string, ITEM_ATTRIBUTES_T>( itemName, { priceCents, units, nullptr } ) );
        result = true;
    }

    return result;
}

bool ItemConfigurationData::Contains( const string& itemName )
{
    bool result = false;

    map<string, ITEM_ATTRIBUTES_T>::iterator itr;

    itr = itemConfigurationDataPtr->find( itemName );

    if ( itr != itemConfigurationDataPtr->end() )
        result = true;

    return result;
}

// Allocates memory for a new SPECIAL_ATTRIBUTES_T, copies the data to it, and assignes its pointer to
// the ITEM_ATTRIBUTES_T associated with itemName.
bool ItemConfigurationData::AddSpecial( const string& itemName, const SPECIAL_ATTRIBUTES_T& specialAttributes )
{
    bool result = false;

    if ( Contains( itemName ) )
    {
        SPECIAL_ATTRIBUTES_T* specialAttrPtr = new SPECIAL_ATTRIBUTES_T;

        memset( specialAttrPtr, 0, sizeof( SPECIAL_ATTRIBUTES_T ) );

        ( * specialAttrPtr ) = specialAttributes;

        itemConfigurationDataPtr->at( itemName ).specialAttributesPtr = specialAttrPtr;

        result = true;
    }

    return result;
}

ITEM_ATTRIBUTES_T ItemConfigurationData::GetItemAttributes( const string& itemName )
{
    return itemConfigurationDataPtr->at( itemName );
}

// Prior to clearing the map<> associated with the configuration data, loop through
// and delete any memory that may have been dynamically allocated for Specials.
void ItemConfigurationData::ClearData( void )
{
    for ( auto itr : ( * itemConfigurationDataPtr ) )
    {
        // Delete dynamically allocated memory
        if ( itr.second.specialAttributesPtr != nullptr )
            delete itr.second.specialAttributesPtr;
    }

    itemConfigurationDataPtr->clear();
}

void ItemConfigurationData::DisplayItemConfigurationData( void )
{
    map<string, ITEM_ATTRIBUTES_T>::iterator itr;

    for ( itr = itemConfigurationDataPtr->begin(); itr != itemConfigurationDataPtr->end(); itr++ )
        DisplayItemConfiguration( itr );
}

void ItemConfigurationData::DisplayItemConfigurationData( const string& itemName )
{
    map<string, ITEM_ATTRIBUTES_T>::iterator itr = itemConfigurationDataPtr->find( itemName );
    DisplayItemConfiguration( itr );
}

// Displays the configuration attributes of a confguration item. This includes the item name,
// unit price, and a description of any associated Specials.
void ItemConfigurationData::DisplayItemConfiguration( map<string, ITEM_ATTRIBUTES_T>::iterator itr )
{
    string buf1, buf2;

    string buf3 = " lesser or equal ";

    cout << itr->first << "\t" << CentsToDollarsString( itr->second.priceCents ) \
        << "\t" << ( ( itr->second.units == ITEM_UNITS_EACH ) ? "EACH" : "LB" );

    if ( itr->second.specialAttributesPtr != nullptr )
    {
        cout << "\t";

        if ( itr->second.specialAttributesPtr->specialType == SPECIAL_BUY_N_GET_M_AT_X_PCT_OFF )
            buf3 = " at ";

        switch ( itr->second.specialAttributesPtr->specialType )
        {
        case SPECIAL_MARK_DOWN:
            cout << itr->second.specialAttributesPtr->pctOff << "% off";
            break;
        case SPECIAL_BUY_N_GET_M_AT_X_PCT_OFF:
        case SPECIAL_BUY_N_GET_M_OF_EQUAL_OR_LESSER_VALUE_FOR_X_PCT_OFF:
            cout << "Buy " << GetQuantityString( itr->second.specialAttributesPtr->buyN, itr->second.units, buf1 ) \
                << " Get " << GetQuantityString( itr->second.specialAttributesPtr->getM, itr->second.units, buf2 ) \
                << buf3 << itr->second.specialAttributesPtr->pctOff << "% off";
            break;
        case SPECIAL_N_FOR_X_DOLLARS:
            cout << GetQuantityString( itr->second.specialAttributesPtr->buyN, itr->second.units, buf1 ) << " for " \
                 << CentsToDollarsString( itr->second.specialAttributesPtr->priceCentsForN );
            break;
        default:
            break;
        }
    }

    cout << endl;
}

string& ItemConfigurationData::GetQuantityString( unsigned int quantity, ITEM_UNITS_T units, string& resultStr )
{
    if ( units == ITEM_UNITS_WEIGHT )
    {
        string temp = to_string( quantity % CENTI_POUNDS_PER_POUND );
        if ( stoi( temp ) < 10 )
            temp.insert( 0, "0" );
        resultStr = to_string( quantity / CENTI_POUNDS_PER_POUND ) + "." + temp;
    }
    else
        resultStr = to_string( quantity );

    return resultStr;
}

///////////////////////////////////////////////////////////////////////////////
//                        CheckoutTotal Class                                //
///////////////////////////////////////////////////////////////////////////////

CheckoutTotal::CheckoutTotal():
    checkoutTotal( 0 )
{
    checkoutTotalDataPtr = new map<string, CheckoutTotal::ITEM_ATTRIBUTES_CHECKOUT_TOTAL_T>;
}

CheckoutTotal::~CheckoutTotal()
{
    ClearData();
    delete checkoutTotalDataPtr;
}

bool CheckoutTotal::Contains( const string& itemName )
{
    bool result = false;

    map<string, CheckoutTotal::ITEM_ATTRIBUTES_CHECKOUT_TOTAL_T>::iterator itr;

    itr = checkoutTotalDataPtr->find( itemName );

    if ( itr != checkoutTotalDataPtr->end() )
        result = true;

    return result;
}

void CheckoutTotal::AddItem( ItemConfigurationData * itemConfigData, const string& itemName, unsigned int quantity )
{
    // If a slot in the current map<> does not exist; create one.
    if ( !Contains( itemName ) )
        checkoutTotalDataPtr->insert( pair<string, CheckoutTotal::ITEM_ATTRIBUTES_CHECKOUT_TOTAL_T>( itemName, { 0, 0 } ) );

    checkoutTotalDataPtr->at( itemName ).quantity += quantity;
    checkoutTotalDataPtr->at( itemName ).groupPrice = GetGroupPrice( itemConfigData, itemName );
    UpdateCheckoutTotal();
}

unsigned int CheckoutTotal::GetGroupPrice( ItemConfigurationData * itemConfigData, const string& itemName )
{
    unsigned int result = 0;
    unsigned int tempResult = 0;
    unsigned int unitsDivider = CENTI_POUNDS_PER_POUND;
    unsigned int groupQuantity = checkoutTotalDataPtr->at( itemName ).quantity;
    ITEM_ATTRIBUTES_T tempItemAttributes = itemConfigData->GetItemAttributes( itemName );

    // In mathematical computations below, fractional pennies are always truncated; favoring of the customer.

    if ( tempItemAttributes.units == ITEM_UNITS_EACH )
        unitsDivider = 1;

    // If there is a Special associated with this item.
    if ( tempItemAttributes.specialAttributesPtr != nullptr )
    {
        switch ( tempItemAttributes.specialAttributesPtr->specialType )
        {
        case SPECIAL_MARK_DOWN:
            result += ComputePriceAfterPctOff( groupQuantity, tempItemAttributes.priceCents,
                                               tempItemAttributes.specialAttributesPtr->pctOff, unitsDivider );
            break;
        case SPECIAL_BUY_N_GET_M_AT_X_PCT_OFF:
        case SPECIAL_BUY_N_GET_M_OF_EQUAL_OR_LESSER_VALUE_FOR_X_PCT_OFF:
            result += Apply_BuyN_GetM_At_X_PctOff( tempItemAttributes, groupQuantity, unitsDivider );
            break;
        case SPECIAL_N_FOR_X_DOLLARS:
            // The unit price is X/N no matter how many the customer buys (less limit applied above).
            tempResult += groupQuantity * tempItemAttributes.specialAttributesPtr->priceCentsForN;
            tempResult /= tempItemAttributes.specialAttributesPtr->buyN;
            result += tempResult;
            break;
        default:
            break;
        }
    }
    else
    {
        result = groupQuantity * tempItemAttributes.priceCents;
        result /= unitsDivider;
    }

    return result;
}

// Buys N, then as much of M as is appropriate for the total quantity. Repeats loop until the quantity
// is consumed. Returns total price paid.
unsigned int CheckoutTotal::Apply_BuyN_GetM_At_X_PctOff( ITEM_ATTRIBUTES_T& itemAttr, unsigned int quantity, unsigned int unitsDivider )
{
    unsigned int result = 0;
    unsigned int tempQuantity = quantity;
    unsigned int tempResult = 0;

    while ( tempQuantity > itemAttr.specialAttributesPtr->buyN )
    {
        // Buy a quantity of N.
        result += ( itemAttr.specialAttributesPtr->buyN * itemAttr.priceCents ) / unitsDivider;
        tempQuantity -= itemAttr.specialAttributesPtr->buyN;

        // If, after buying N, the remaining quantity is less than M,  buy only that quantity
        // at the discounted price. Otherwise buy M at the discounted price.
        if ( tempQuantity < itemAttr.specialAttributesPtr->getM )
        {
            tempResult = ComputePriceAfterPctOff( tempQuantity, itemAttr.priceCents,
                                                  itemAttr.specialAttributesPtr->pctOff, unitsDivider );
            tempQuantity = 0;
        }
        else
        {
            tempResult = ComputePriceAfterPctOff( itemAttr.specialAttributesPtr->getM, itemAttr.priceCents,
                                                  itemAttr.specialAttributesPtr->pctOff, unitsDivider );
            tempQuantity -= itemAttr.specialAttributesPtr->getM ;
        }
        result += tempResult;
    }

    // The remainder are charged the regular price.
    result += ( tempQuantity * itemAttr.priceCents ) / unitsDivider;

    return result;
}

unsigned int CheckoutTotal::ComputePriceAfterPctOff( unsigned int quantity, unsigned int unitPrice,
                                                     unsigned int pctOff, unsigned int unitsDivider )
{
    unsigned int discountUnitPrice = unitPrice * ( 100 - pctOff );
    discountUnitPrice /= CENTS_PER_DOLLAR;

    unsigned int result = quantity * discountUnitPrice;
    result /= unitsDivider;

    return result;
}

bool CheckoutTotal::RemoveItem( ItemConfigurationData * itemConfigData, string itemName, unsigned int quantity )
{
    bool result = false;

    if ( Contains( itemName ) && checkoutTotalDataPtr->at( itemName ).quantity >= quantity )
    {
        if ( checkoutTotalDataPtr->at( itemName ).quantity == quantity )
            checkoutTotalDataPtr->erase( itemName );
        else
        {
            checkoutTotalDataPtr->at( itemName ).quantity -= quantity;
            checkoutTotalDataPtr->at( itemName ).groupPrice = GetGroupPrice( itemConfigData, itemName );
        }

        result = true;
    }

    UpdateCheckoutTotal();

    return result;
}

void CheckoutTotal::UpdateCheckoutTotal( void )
{
    checkoutTotal = 0;

    for ( auto itr : ( * checkoutTotalDataPtr ) )
        checkoutTotal += itr.second.groupPrice;
}

void CheckoutTotal::ClearData( void )
{
    checkoutTotalDataPtr->clear();
    checkoutTotal = 0;
}


///////////////////////////////////////////////////////////////////////////////
//                      CheckoutTotalTest Class                              //
///////////////////////////////////////////////////////////////////////////////

CheckoutTotalTest::CheckoutTotalTest():
    overallResult( false )
{
    itemConfigurationData = new ItemConfigurationData;
    checkoutTotal = new CheckoutTotal;

    // vector< tuple< itemName, units, priceCents > >
    itemConfigTestDataVector = new vector< tuple <string, ITEM_UNITS_T, unsigned int> >;

    // vector< tuple< itemName, specialType, buyN, getM, pctOff OR priceCentsForN > >
    itemConfigSpecialTestDataVector = new vector< tuple <string, SPECIAL_TYPE_T, unsigned int, unsigned int, unsigned int> >;

    // vector< tuple< itemName, add (remove), quantity, checkoutTotal > >
    checkoutTotalTestDataVector = new vector< tuple <string, bool, unsigned int, unsigned int> >;

    InitializeTestDataVectors();
}

CheckoutTotalTest::~CheckoutTotalTest()
{
    itemConfigurationData->ClearData();
    checkoutTotal->ClearData();
    itemConfigTestDataVector->clear();
    itemConfigSpecialTestDataVector->clear();
    checkoutTotalTestDataVector->clear();
    delete itemConfigurationData;
    delete checkoutTotal;
    delete itemConfigTestDataVector;
    delete itemConfigSpecialTestDataVector;
    delete checkoutTotalTestDataVector;
}

void CheckoutTotalTest::RunTests()
{
    overallResult = true;

    AddAndReadBackTestOfItemConfiguration();
    AddAndReadBackTestOfSpecialConfiguration();
    CheckoutRunningTotalTest();

    overallResult ? DisplayTestResultsBanner( "Overall Result: PASS" ) : DisplayTestResultsBanner( "Overall Result : FAIL" );
}

// Adds the constant item test data in the itemConfigTestDataVector to the itemConfigurationData.
// Reads back the data and compares it to the original constant item test data. This verifies that data
// can be setup in the configuration object as required.
void CheckoutTotalTest::AddAndReadBackTestOfItemConfiguration( void )
{
    itemConfigurationData->ClearData();

    DisplayTestResultsBanner( "Add/Read-Back Test of Item Configuration" );

    // Add the items from the test vector to the configuration.
    for ( auto itr : ( * itemConfigTestDataVector ) )
        itemConfigurationData->AddItem( get<0>( itr ), get<1>( itr ), get<2>( itr ) );

    // Read back the items in the configuration and compare to the test vector.
    for ( auto itr : ( * itemConfigTestDataVector ) )
    {
        cout << get<0>( itr );
        if ( !itemConfigurationData->Contains( get<0>( itr ) ) )
        {
            cout << " not found.";
            PRINT_FAIL_END;
        }
        else
        {
            cout << " found.";
            PRINT_PASS_END;

            ( void ) ActualVsExpected( get<0>( itr ), "Units", ( unsigned int ) get<1>( itr ), ( unsigned int ) itemConfigurationData->GetItemAttributes( get<0>( itr ) ).units );
            ( void ) ActualVsExpected( get<0>( itr ), "priceCents", get<2>( itr ), itemConfigurationData->GetItemAttributes( get<0>( itr ) ).priceCents );
        }
    }
}

// Adds the constant Special test data in the itemConfigSpecialTestDataVector to the itemConfigurationData.
// Reads back the data and compares it to the original constant Special test data. This verifies that data
// can be setup in the configuration object as required.
void CheckoutTotalTest::AddAndReadBackTestOfSpecialConfiguration( void )
{
    SPECIAL_ATTRIBUTES_T specialAttributes;

    memset( &specialAttributes, 0, sizeof( SPECIAL_ATTRIBUTES_T ) );

    DisplayTestResultsBanner( "Add/Read-Back Test of Item Special Configuration" );

    // Configure the Special test data.
    // vector< tuple< itemName, specialType, buyN, getM, pctOff > >
    for ( auto itr : ( * itemConfigSpecialTestDataVector ) )
    {
        specialAttributes.specialType = get<1>( itr );

        switch ( get<1>( itr ) )
        {
        case SPECIAL_MARK_DOWN:
            specialAttributes.pctOff = get<4>( itr );
            break;
        case SPECIAL_N_FOR_X_DOLLARS:
            specialAttributes.buyN = get<2>( itr );
            specialAttributes.priceCentsForN = get<4>( itr );
            break;
        case SPECIAL_BUY_N_GET_M_AT_X_PCT_OFF:
        case SPECIAL_BUY_N_GET_M_OF_EQUAL_OR_LESSER_VALUE_FOR_X_PCT_OFF:
            specialAttributes.buyN = get<2>( itr );
            specialAttributes.getM = get<3>( itr );
            specialAttributes.pctOff = get<4>( itr );
            break;

        default:
            cout << "Special Type not found.";
            PRINT_FAIL_END;
            break;
        }

        ( void ) itemConfigurationData->AddSpecial( get<0>( itr ), specialAttributes );
    }

    // Read back and verify the Special data.
    // vector< tuple< itemName, specialType, pctOff> >
    for ( auto itr : ( * itemConfigSpecialTestDataVector ) )
    {
        cout << get<0>( itr ) << " Special";
        if ( !itemConfigurationData->Contains( get<0>( itr ) ) )
        {
            cout << ": Attempt to add special to nonexistent item.";
            PRINT_FAIL_END;
        }
        else if ( itemConfigurationData->GetItemAttributes( get<0>( itr ) ).specialAttributesPtr == nullptr )
        {
            cout << " not found.";
            PRINT_FAIL_END;
        }
        else
        {
            cout << " found."; PRINT_PASS_END;

            if ( ActualVsExpected( get<0>( itr ), "specialType", ( unsigned int ) get<1>( itr ), ( unsigned int ) itemConfigurationData->GetItemAttributes( get<0>( itr ) ).specialAttributesPtr->specialType ) )
            {
                ( void ) ActualVsExpected( get<0>( itr ), "buyN", get<2>( itr ), itemConfigurationData->GetItemAttributes( get<0>( itr ) ).specialAttributesPtr->buyN );
                ( void ) ActualVsExpected( get<0>( itr ), "getM", get<3>( itr ), itemConfigurationData->GetItemAttributes( get<0>( itr ) ).specialAttributesPtr->getM );

                switch ( get<1>( itr ) )
                {
                case SPECIAL_N_FOR_X_DOLLARS:
                    ( void ) ActualVsExpected( get<0>( itr ), "priceCentsForN", get<4>( itr ), itemConfigurationData->GetItemAttributes( get<0>( itr ) ).specialAttributesPtr->priceCentsForN );
                    break;
                case SPECIAL_MARK_DOWN:
                case SPECIAL_BUY_N_GET_M_AT_X_PCT_OFF:
                case SPECIAL_BUY_N_GET_M_OF_EQUAL_OR_LESSER_VALUE_FOR_X_PCT_OFF:
                    ( void ) ActualVsExpected( get<0>( itr ), "pctOff", get<4>( itr ), itemConfigurationData->GetItemAttributes( get<0>( itr ) ).specialAttributesPtr->pctOff );
                    break;
                default:
                    cout << "Special Type not found.";
                    PRINT_FAIL_END;
                    break;
                }
            }
        }
    }
}

void CheckoutTotalTest::CheckoutRunningTotalTest( void )
{
    checkoutTotal->ClearData();
    DisplayTestResultsBanner( "Running Checkout Total Test" );

    // vector< tuple< itemName, add (remove), quantity, checkoutTotal > >
    for ( auto itr : ( * checkoutTotalTestDataVector ) )
    {
        if ( get<1>( itr ) )
            checkoutTotal->AddItem( itemConfigurationData, get<0>( itr ), get<2>( itr ) );
        else
            checkoutTotal->RemoveItem( itemConfigurationData, get<0>( itr ), get<2>( itr ) );

        ActualVsExpected( get<0>( itr ), "checkoutTotal", get<3>( itr ), checkoutTotal->GetCheckoutTotal() );
    }
}

bool CheckoutTotalTest::ActualVsExpected( const string& itemName, const string& paramName, unsigned int expected, unsigned int actual )
{
    bool result = false;

    cout << itemName << " " << paramName << " expect " << expected << " got " << actual;
    if ( actual == expected )
    {
        PRINT_PASS_END;
        result = true;
    }
    else
        PRINT_FAIL_END;

    return result;
}

void CheckoutTotalTest::DisplayTestResultsBanner( const string& title )
{
    cout << endl << "///////////////////////////////////////////////////////////////////////////////" << endl;
    cout << "//" << endl;
    cout << "//" << " " << title << endl;
    cout << "//" << endl;
    cout << "///////////////////////////////////////////////////////////////////////////////" << endl << endl;
}

// Sets up the pre-determined data to run the tests against.
void CheckoutTotalTest::InitializeTestDataVectors( void )
{
    ///////////////////////////////////////////////////////////////////////////
    // Initialize vector used to store test data for Item Configuration
    // add/read-back test.

    // Items without specials assigned.
    itemConfigTestDataVector->push_back( make_tuple( "Corn Flakes", ITEM_UNITS_EACH, 298 ) );
    itemConfigTestDataVector->push_back( make_tuple( "Ground Beef", ITEM_UNITS_WEIGHT, 499 ) );
    // Items with specials assigned.
    itemConfigTestDataVector->push_back( make_tuple( "Whole Grain Bread", ITEM_UNITS_EACH, 249 ) );
    itemConfigTestDataVector->push_back( make_tuple( "Deli Turkey", ITEM_UNITS_WEIGHT, 599 ) );
    itemConfigTestDataVector->push_back( make_tuple( "Walnuts", ITEM_UNITS_EACH, 597 ) );
    itemConfigTestDataVector->push_back( make_tuple( "Pears", ITEM_UNITS_WEIGHT, 129 ) );
    itemConfigTestDataVector->push_back( make_tuple( "Blueberries (pint)", ITEM_UNITS_EACH, 399 ) );
    itemConfigTestDataVector->push_back( make_tuple( "Russet Potatoes", ITEM_UNITS_WEIGHT, 79 ) );
    itemConfigTestDataVector->push_back( make_tuple( "Deli Cheddar Cheese", ITEM_UNITS_WEIGHT, 199 ) );


    ///////////////////////////////////////////////////////////////////////////
    // Initialize vector used to store test data for Item Special Configuration
    // add/read-back test.

    // SPECIAL_MARK_DOWN, weighted and unweighted items.
    itemConfigSpecialTestDataVector->push_back( make_tuple( "Walnuts", SPECIAL_MARK_DOWN, 0, 0, 25 ) );
    itemConfigSpecialTestDataVector->push_back( make_tuple( "Pears", SPECIAL_MARK_DOWN, 0, 0, 15 ) );

    // SPECIAL_N_FOR_X_DOLLARS, weighted and unweighted items.
    itemConfigSpecialTestDataVector->push_back( make_tuple( "Blueberries (pint)", SPECIAL_N_FOR_X_DOLLARS, 2, 0, 500 ) );
    itemConfigSpecialTestDataVector->push_back( make_tuple( "Russet Potatoes", SPECIAL_N_FOR_X_DOLLARS, 100, 0, 59 ) );

    // SPECIAL_BUY_N_GET_M_AT_X_PCT_OFF, weighted and unweighted items.
    itemConfigSpecialTestDataVector->push_back( make_tuple( "Whole Grain Bread", SPECIAL_BUY_N_GET_M_AT_X_PCT_OFF, 1, 1, 100 ) );
    itemConfigSpecialTestDataVector->push_back( make_tuple( "Deli Cheddar Cheese", SPECIAL_BUY_N_GET_M_AT_X_PCT_OFF, 200, 100, 25 ) );

    // SPECIAL_BUY_N_GET_M_OF_EQUAL_OR_LESSER_VALUE_FOR_X_PCT_OFF, weighted items only.
    itemConfigSpecialTestDataVector->push_back( make_tuple( "Deli Turkey", SPECIAL_BUY_N_GET_M_OF_EQUAL_OR_LESSER_VALUE_FOR_X_PCT_OFF, 200, 100, 50 ) );


    ///////////////////////////////////////////////////////////////////////////
    // Initialize vector used to store test data for Checkout Running Total test.
    //
    // NOTE: Anytime a special is calculated, unit price fractional pennies are 
    //       truncated; in favor of the customer.

    checkoutTotalTestDataVector->push_back( make_tuple( "Ground Beef", true, 200, 998 ) ); // 998
    checkoutTotalTestDataVector->push_back( make_tuple( "Corn Flakes", true, 1, 1296 ) ); // 298
    checkoutTotalTestDataVector->push_back( make_tuple( "Corn Flakes", true, 2, 1892 ) ); // 596
    checkoutTotalTestDataVector->push_back( make_tuple( "Ground Beef", true, 153, 2655 ) ); // 763
    checkoutTotalTestDataVector->push_back( make_tuple( "Corn Flakes", false, 1, 2357 ) ); // 298
    checkoutTotalTestDataVector->push_back( make_tuple( "Ground Beef", false, 200, 1359 ) ); // 998
    checkoutTotalTestDataVector->push_back( make_tuple( "Corn Flakes", false, 1, 1061 ) ); // 298
    checkoutTotalTestDataVector->push_back( make_tuple( "Ground Beef", false, 153, 298 ) ); // 763
    checkoutTotalTestDataVector->push_back( make_tuple( "Corn Flakes", false, 1, 0 ) ); // 298

    // Test SPECIAL_MARK_DOWN items.

    // Regular price  = 5.97
    // Discount price = 5.97 * 0.75 = 4.47
    checkoutTotalTestDataVector->push_back( make_tuple( "Walnuts", true, 1, 447 ) );
    checkoutTotalTestDataVector->push_back( make_tuple( "Walnuts", true, 2, 1341 ) );
    checkoutTotalTestDataVector->push_back( make_tuple( "Walnuts", false, 1, 894 ) );
    checkoutTotalTestDataVector->push_back( make_tuple( "Walnuts", false, 1, 447 ) );
    checkoutTotalTestDataVector->push_back( make_tuple( "Walnuts", false, 1, 0 ) );

    // Regular price  = 1.29
    // Discount price = 1.29 * 0.85 = 1.09
    checkoutTotalTestDataVector->push_back( make_tuple( "Pears", true, 100, 109 ) );
    // (1.0 + 1.21) * 109 = 240
    checkoutTotalTestDataVector->push_back( make_tuple( "Pears", true, 121, 240 ) );
    // (1.0 + 1.21 + 0.67) * 109 = 240
    checkoutTotalTestDataVector->push_back( make_tuple( "Pears", true, 67, 313 ) );
    // (1.21) * 109 = 240
    checkoutTotalTestDataVector->push_back( make_tuple( "Pears", false, 167, 131 ) );
    checkoutTotalTestDataVector->push_back( make_tuple( "Pears", false, 121, 0 ) );

    // Test SPECIAL_N_FOR_X_DOLLARS items.

    checkoutTotalTestDataVector->push_back( make_tuple( "Blueberries (pint)", true, 1, 250 ) );
    checkoutTotalTestDataVector->push_back( make_tuple( "Blueberries (pint)", true, 1, 500 ) );
    checkoutTotalTestDataVector->push_back( make_tuple( "Blueberries (pint)", true, 1, 750 ) );
    checkoutTotalTestDataVector->push_back( make_tuple( "Blueberries (pint)", true, 1, 1000 ) );
    checkoutTotalTestDataVector->push_back( make_tuple( "Blueberries (pint)", false, 2, 500 ) );
    checkoutTotalTestDataVector->push_back( make_tuple( "Blueberries (pint)", false, 2, 0 ) );

    // Price for new total = 0.910 * 59 = 53
    checkoutTotalTestDataVector->push_back( make_tuple( "Russet Potatoes", true, 91, 53 ) );
    // Price for new total = (0.910 + 2.13) * 59 = 179
    checkoutTotalTestDataVector->push_back( make_tuple( "Russet Potatoes", true, 213, 179 ) );
    // Price for new total = (0.910 + 2.13 + 1.33) * 59 = 257
    checkoutTotalTestDataVector->push_back( make_tuple( "Russet Potatoes", true, 133, 257 ) );
    // Price for new total = (0.910 + 1.33) * 59 = 132
    checkoutTotalTestDataVector->push_back( make_tuple( "Russet Potatoes", false, 213, 132 ) );
    // Price for new total = 1.33 * 59 = 78
    checkoutTotalTestDataVector->push_back( make_tuple( "Russet Potatoes", false, 91, 78 ) );
    checkoutTotalTestDataVector->push_back( make_tuple( "Russet Potatoes", false, 133, 0 ) );

    // Test SPECIAL_BUY_N_GET_M_AT_X_PCT_OFF items.

    checkoutTotalTestDataVector->push_back( make_tuple( "Whole Grain Bread", true, 1, 249 ) );
    checkoutTotalTestDataVector->push_back( make_tuple( "Whole Grain Bread", true, 1, 249 ) );
    checkoutTotalTestDataVector->push_back( make_tuple( "Whole Grain Bread", true, 1, 498 ) );
    checkoutTotalTestDataVector->push_back( make_tuple( "Whole Grain Bread", true, 1, 498 ) );
    checkoutTotalTestDataVector->push_back( make_tuple( "Whole Grain Bread", false, 2, 249 ) );
    checkoutTotalTestDataVector->push_back( make_tuple( "Whole Grain Bread", false, 2, 0 ) );

    // Reglar price: 1.99 / lb
    // Discount price = 1.99 * 0.75 = 1.49 / lb
    //
    // x < 200
    // Price for new total = 1.53 * 199 = 304
    checkoutTotalTestDataVector->push_back( make_tuple( "Deli Cheddar Cheese", true, 153, 304 ) );
    // 200 < x < 300
    // Price for new total =  2.0 * 199 + (1.53 + 0.75 - 2.0) * 149 = 398 + 41 = 439
    checkoutTotalTestDataVector->push_back( make_tuple( "Deli Cheddar Cheese", true, 75, 439 ) );
    // 300 < x < 500
    // Price for new total =  2.0 * 199 + 1.0 * 149 + (1.53 + 0.75 + 2.15 - 3.0) * 199 = 398 + 149 + 284 = 831
    checkoutTotalTestDataVector->push_back( make_tuple( "Deli Cheddar Cheese", true, 215, 831 ) );
    // 600 < x < 800
    // Price for new total =  4.0 * 199 + 2.0 * 149 + (1.53 + 0.75 + 2.15 + 2.03 - 6.0) * 199 = 796 + 298 + 91 = 1185
    checkoutTotalTestDataVector->push_back( make_tuple( "Deli Cheddar Cheese", true, 203, 1185 ) );
    // 500 < x < 600
    // Price for new total =  4.0 * 199 + 1.0 * 149 + (1.53 + 2.15 + 2.03 - 5.0 ) * 149 = 796 + 149 + 105 = 1050
    checkoutTotalTestDataVector->push_back( make_tuple( "Deli Cheddar Cheese", false, 75, 1050 ) );
    // x < 200
    // Price for new total = 1.53 * 199 = 304
    checkoutTotalTestDataVector->push_back( make_tuple( "Deli Cheddar Cheese", false, 418, 304 ) );
    checkoutTotalTestDataVector->push_back( make_tuple( "Deli Cheddar Cheese", false, 153, 0 ) );

    // Test SPECIAL_BUY_N_GET_M_OF_EQUAL_OR_LESSER_VALUE_FOR_X_PCT_OFF items.

    // Reglar price: 5.99 / lb
    // Discount price = 5.99 * 0.50 = 2.99 / lb
    //
    // x < 200
    // Price for new total = 1.87 * 5.99 * 100 = 608
    checkoutTotalTestDataVector->push_back( make_tuple( "Deli Turkey", true, 187, 1120 ) );
    // 200 < x < 300
    // Price for new total =  2.0 * 599 + (1.87 + 0.63 - 2.0) * 299 = 1198 + 149 = 1347
    checkoutTotalTestDataVector->push_back( make_tuple( "Deli Turkey", true, 63, 1347 ) );
    // 300 < x < 500
    // Price for new total =  2.0 * 599 + 1.0 * 299 + (1.87 + 0.63 + 2.11 - 3.0) * 599 = 1198 + 299 + 964 = 2461
    checkoutTotalTestDataVector->push_back( make_tuple( "Deli Turkey", true, 211, 2461 ) );
    // 500 < x < 600
    // Price for new total =  4.0 * 599 + 1.0 * 299 + (1.87 + 0.63 + 2.11 + 1.3 - 5.0) * 299 = 2396 + 299 + 272 = 2967
    checkoutTotalTestDataVector->push_back( make_tuple( "Deli Turkey", true, 130, 2967 ) );
    // 600 < x < 800
    // Price for new total =  4.0 * 599 + 2.0 * 299 + (1.87 + 0.63 + 2.11 + 1.3 + 0.33 - 6.0) * 599 = 2396 + 598 + 143 = 3137
    checkoutTotalTestDataVector->push_back( make_tuple( "Deli Turkey", true, 33, 3137 ) );
    checkoutTotalTestDataVector->push_back( make_tuple( "Deli Turkey", false, 624, 0 ) );

}

// NOTES:
// g++ -std=c++11 -Wall -Wextra -pedantic-errors Checkout.cpp -o Checkout
