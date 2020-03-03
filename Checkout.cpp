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
    SPECIAL_MARK_DOWN
} SPECIAL_TYPE_T;

typedef struct
{
    SPECIAL_TYPE_T specialType;

    unsigned int pctOff;

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

    // vector< tuple< itemName, specialType, pctOff > >
    vector< tuple <string, SPECIAL_TYPE_T, unsigned int> > * itemConfigSpecialTestDataVector;

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
        cout << "Enter 0: ";

        getline( cin >> ws, inBuf1 );

        if ( !ValidateNumericEntryString( inBuf1, &tempInt, 0, 0 ) )
            INVALID_ENTRY;
        else
        {
            specialAttributes.specialType = ( SPECIAL_TYPE_T ) tempInt;

            cout << "Enter pctOff (1 - 100): ";
            getline( cin >> ws, inBuf1 );
            if ( !ValidateNumericEntryString( inBuf1, &tempInt, 1, 100 ) )
                allEntriesAreValid = false;
            else
                specialAttributes.pctOff = tempInt;

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
    cout << itr->first << "\t" << CentsToDollarsString( itr->second.priceCents ) \
        << "\t" << ( ( itr->second.units == ITEM_UNITS_EACH ) ? "EACH" : "LB" );

    if ( itr->second.specialAttributesPtr != nullptr )
    {
        cout << "\t";

        switch ( itr->second.specialAttributesPtr->specialType )
        {
        case SPECIAL_MARK_DOWN:
            cout << itr->second.specialAttributesPtr->pctOff << "% off";
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

    // vector< tuple< itemName, specialType, pctOff > >
    itemConfigSpecialTestDataVector = new vector< tuple <string, SPECIAL_TYPE_T, unsigned int> >;

    // vector< tuple< itemName, add, quantity, checkoutTotal > >
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
    // vector< tuple< itemName, specialType, pctOff > >
    for ( auto itr : ( * itemConfigSpecialTestDataVector ) )
    {
        switch ( get<1>( itr ) )
        {
        case SPECIAL_MARK_DOWN:
            specialAttributes.specialType = get<1>( itr );
            specialAttributes.pctOff = get<2>( itr );
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
                switch ( get<1>( itr ) )
                {
                case SPECIAL_MARK_DOWN:
                    ( void ) ActualVsExpected( get<0>( itr ), "pctOff", get<2>( itr ), itemConfigurationData->GetItemAttributes( get<0>( itr ) ).specialAttributesPtr->pctOff );
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
    itemConfigTestDataVector->push_back( make_tuple( "Walnuts", ITEM_UNITS_EACH, 597 ) );
    itemConfigTestDataVector->push_back( make_tuple( "Pears", ITEM_UNITS_WEIGHT, 129 ) );


    ///////////////////////////////////////////////////////////////////////////
    // Initialize vector used to store test data for Item Special Configuration
    // add/read-back test.

    // SPECIAL_MARK_DOWN, weighted and unweighted items.
    itemConfigSpecialTestDataVector->push_back( make_tuple( "Walnuts", SPECIAL_MARK_DOWN, 25 ) );
    itemConfigSpecialTestDataVector->push_back( make_tuple( "Pears", SPECIAL_MARK_DOWN, 15 ) );


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

}

// NOTES:
// g++ -std=c++11 -Wall -Wextra -pedantic-errors Checkout.cpp -o Checkout
