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

typedef struct
{
    unsigned int priceCents;
    ITEM_UNITS_T    units;
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

    // vector< tuple< itemName, add (remove), quantity, checkoutTotal > >
    vector< tuple <string, unsigned int, unsigned int> > * checkoutTotalTestDataVector;

    bool ActualVsExpected( const string& itemName, const string& paramName, unsigned int expected, unsigned int actual );
    void DisplayTestResultsBanner( const string& title );
    void AddAndReadBackTestOfItemConfiguration( void );
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
void ReadCheckoutItemAdd( ItemConfigurationData * itemConfigurationData, CheckoutTotal * checkoutTotal );
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
        cin >> inBuf1;

        if ( inBuf1 == "ci" ) // configitem
            ReadItemConfiguration( itemConfigurationData );
        else if ( inBuf1 == "lc" ) // listconfig
            itemConfigurationData->DisplayItemConfigurationData();
        else if ( inBuf1 == "ai" ) // additem
            ReadCheckoutItemAdd( itemConfigurationData, checkoutTotal );
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

void ReadCheckoutItemAdd( ItemConfigurationData * itemConfigurationData, CheckoutTotal * checkoutTotal )
{
    string itemName, tempBuf;
    unsigned int tempInt;
    unsigned int prevCheckoutTotal = checkoutTotal->GetCheckoutTotal();

    cout << "Enter Item name: ";
    getline( cin >> ws, itemName );

    if ( !itemConfigurationData->Contains( itemName ) )
    {
        cout << itemName << " does not exist." << endl;
    }
    else
    {
        if ( !ReadQuantityParam( &tempInt, itemConfigurationData->GetItemAttributes( string( itemName ) ).units, "" ) )
            INVALID_ENTRY;
        else
        {

            checkoutTotal->AddItem( itemConfigurationData, itemName, tempInt );

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
        itemConfigurationDataPtr->insert( pair<string, ITEM_ATTRIBUTES_T>( itemName, { priceCents, units } ) );
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

ITEM_ATTRIBUTES_T ItemConfigurationData::GetItemAttributes( const string& itemName )
{
    return itemConfigurationDataPtr->at( itemName );
}

void ItemConfigurationData::ClearData( void )
{
    itemConfigurationDataPtr->clear();
}

void ItemConfigurationData::DisplayItemConfigurationData( void )
{
    map<string, ITEM_ATTRIBUTES_T>::iterator itr;

    for ( itr = itemConfigurationDataPtr->begin(); itr != itemConfigurationDataPtr->end(); itr++ )
    {
        DisplayItemConfiguration( itr );
    }
}

void ItemConfigurationData::DisplayItemConfigurationData( const string& itemName )
{
    map<string, ITEM_ATTRIBUTES_T>::iterator itr = itemConfigurationDataPtr->find( itemName );
    DisplayItemConfiguration( itr );
}

void ItemConfigurationData::DisplayItemConfiguration( map<string, ITEM_ATTRIBUTES_T>::iterator itr )
{
    string buf1, buf2;

    cout << itr->first << "\t" << CentsToDollarsString( itr->second.priceCents ) \
        << "\t" << ( ( itr->second.units == ITEM_UNITS_EACH ) ? "EACH" : "LB" );

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
    ITEM_UNITS_T tempItemUnits = itemConfigData->GetItemAttributes( itemName ).units;

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

    result = groupQuantity * tempItemAttributes.priceCents;
    result /= unitsDivider;

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

    // vector< tuple< itemName, add, quantity, checkoutTotal > >
    checkoutTotalTestDataVector = new vector< tuple <string, unsigned int, unsigned int> >;

    InitializeTestDataVectors();
}

CheckoutTotalTest::~CheckoutTotalTest()
{
    itemConfigurationData->ClearData();
    checkoutTotal->ClearData();
    itemConfigTestDataVector->clear();
    checkoutTotalTestDataVector->clear();
    delete itemConfigurationData;
    delete checkoutTotal;
    delete itemConfigTestDataVector;
    delete checkoutTotalTestDataVector;
}

void CheckoutTotalTest::RunTests()
{
    overallResult = true;

    AddAndReadBackTestOfItemConfiguration();
    CheckoutRunningTotalTest();

    overallResult ? DisplayTestResultsBanner( "Overall Result: PASS" ) : DisplayTestResultsBanner( "Overall Result : FAIL" );
}

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

void CheckoutTotalTest::CheckoutRunningTotalTest( void )
{
    checkoutTotal->ClearData();
    DisplayTestResultsBanner( "Running Checkout Total Test" );

    // vector< tuple< itemName, add (remove), quantity, checkoutTotal > >
    for ( auto itr : ( * checkoutTotalTestDataVector ) )
    {

        checkoutTotal->AddItem( itemConfigurationData, get<0>( itr ), get<1>( itr ) );

        ActualVsExpected( get<0>( itr ), "checkoutTotal", get<2>( itr ), checkoutTotal->GetCheckoutTotal() );
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

void CheckoutTotalTest::InitializeTestDataVectors( void )
{
    ///////////////////////////////////////////////////////////////////////////
    // Initialize vector used to store test data for Item Configuration
    // add/read-back test.

    // Items.
    itemConfigTestDataVector->push_back( make_tuple( "Corn Flakes", ITEM_UNITS_EACH, 298 ) );
    itemConfigTestDataVector->push_back( make_tuple( "Ground Beef", ITEM_UNITS_WEIGHT, 499 ) );


    ///////////////////////////////////////////////////////////////////////////
    // Initialize vector used to store test data for Checkout Running Total test.
    //
    // NOTE: Anytime a special is calculated, unit price fractional pennies are 
    //       truncated; in favor of the customer.

    checkoutTotalTestDataVector->push_back( make_tuple( "Ground Beef", 200, 998 ) ); // 998
    checkoutTotalTestDataVector->push_back( make_tuple( "Corn Flakes", 1, 1296 ) ); // 298
    checkoutTotalTestDataVector->push_back( make_tuple( "Corn Flakes", 2, 1892 ) ); // 298
    checkoutTotalTestDataVector->push_back( make_tuple( "Ground Beef", 153, 2655 ) ); // 998
}
