#include <fmt/core.h>
#include <pazusoba/route.h>

namespace pazusoba {
void Route::addNextStep(constant::Direction direction) {
    if (_totalSteps == MAX_STEPS)
        throw std::out_of_range(
            fmt::format("pazusoba::Route exceeded max steps {}", MAX_STEPS));

    _steps[_totalSteps] = direction;
    _totalSteps += 1;
}

void Route::printRoute() {
    fmt::print("{} -", _totalSteps);
    for (pint i = 0; i < _totalSteps; i++) {
        fmt::print("{} ", constant::DIRECTION_STRING[_steps[i]]);
    }
    fmt::print("\n");
}

void Route::writeToDisk() {
    // Based on the first location, work out everything
    // TODO: maybe simply save the current index instead??
    // The route doesn't need to knwo about row and column
    // This can be calculated from the python side?
    // Row and Column should be included?
}

void Route::goGack(pint steps) {
    if (steps > _totalSteps)
        throw std::logic_error("pazusoba::Board goBack too many steps");
    _totalSteps -= steps;
}

void Route::shorten() {
    //
}

constant::Direction& Route::operator[](pint index) {
    if (index >= _totalSteps)
        throw std::out_of_range(
            fmt::format("pazusoba::Route index {} out of range {}", index,
                        _totalSteps - 1));

    return _steps[index];
}
};  // namespace pazusoba
