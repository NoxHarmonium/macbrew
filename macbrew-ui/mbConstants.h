
// ALRT
#define kAlertId 128

// WIND
#define kSplashWindowId 128
#define kViewSessionWindowId 129
#define kViewStepsWindowId 130

// PICT
#define kSplashImageId 128

// CNTL
#define kSessionListCancelButtonId 128
#define kSessionListOkButtonId 129

// DLOG
#define kSessionListDialog 128

// DITL
#define kAlertDitlId 128
#define kSessionListDitlId 128

// Control Part Codes
#define inButton 10
#define inCheckbox 11
#define inUpButton 20
#define inDownButton 21
#define inPageUp 22
#define inPageDown 23
#define inThumb 129

// Keyboard Codes
// TODO: This is surely defined somewhere
#define kEnterCharCode 3
#define kReturnCharCode 13
#define kEscapeCharCode 27
#define kPeriodCharCode 46
#define kLeftArrow 28
#define kRightArrow 29
#define kUpArrow 30
#define kDownArrow 31

// TODO: Should be defined in MacWindows.h
#define kDialogWindowKind 2

#define kSessionListUserItem 3

// Fermentation Graph
#define kGraphWidth 320
#define kGraphHeight 180

// Scrollbar Constants
#define kScrollbarWidth 16					   // conventional width
#define kScrollbarAdjust (kScrollbarWidth - 1) // to align with window frame
#define kScrollTweek 2						   // to align scroll bars with size box

// Brew Session Steps
#define kBrewSessionStepHeight 20
#define kBrewSessionWindowHPadding 10
#define kBrewSessionWindowVPadding 0
#define kBrewSessionScrollPageOverlap 20 // The amount of overlap of the previous page to show to provide context when paging

// Window Constants
#define kMinWindowSize 64
// Docs say 65,535 can be used as max but it seems to overflow the short data type and break the resizing
#define kMaxWindowSize 512
