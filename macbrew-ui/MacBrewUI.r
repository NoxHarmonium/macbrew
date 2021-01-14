#include "systypes.r"
#include "types.r"
#include "mbConstants.h"

resource 'vers' (1) {
	0x02, 0x00, release, 0x00,
	verUS,
	"0.12",
	"0.12, Copyright \251 2021 Sean Dawson"
};

resource 'ALRT' (kAlertId, purgeable) {
	{94, 80, 183, 438},
	kAlertId,
	{ 
		OK, visible, sound1,
		OK, visible, sound1,
		OK, visible, sound1,
		OK, visible, sound1,
	},
	alertPositionParentWindow
};