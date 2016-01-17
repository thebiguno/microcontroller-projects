#include "UniversalController.h"

#include "../Chiindii.h"

using namespace digitalcave;

UniversalController::UniversalController(Chiindii *chiindii) {
	this->chiindii = chiindii;
}

void UniversalController::dispatch(FramedSerialMessage* message) {

}
