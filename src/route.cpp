#include <fmt/core.h>
#include <pazusoba/route.h>
#include <cstdio>

namespace pazusoba {
void Route::addNextStep(pint index) {
    _list.emplace_back(index);
    _totalSteps += 1;
}

void Route::printRoute() const {
    // This includes the initial step
    fmt::print("{} step(s) - ", _totalSteps - 1);
    for (pint i = 0; i < _totalSteps; i++) {
        auto index = _list[i];
        auto x = index / _column;
        auto y = index % _column;
        fmt::print("({}, {}) -> ", x, y);
    }
    fmt::print("END\n");
}

void Route::writeToDisk() {
    // 1,1|1,2|... the same format as before
    // we can optimise it first before writing
    shorten();

    // fmt/os doesn't compile on Windows
    FILE* path = fopen("path.pazusoba", "w");
    // consider cases where the program doesn't have
    // the permission to write to disk, like Android
    if (path != nullptr) {
        for (pint i = 0; i < _totalSteps; i++) {
            auto index = _list[i];
            auto x = index / _column;
            auto y = index % _column;
            fprintf(path, "%d,%d|", x, y);
        }

        fclose(path);
    }
}

void Route::goGack(pint steps) {
    if (steps > _totalSteps)
        throw std::logic_error("pazusoba::Board goBack too many steps");
    for (pint i = 0; i < steps; i++) {
        _list.pop_back();
        _totalSteps--;
    }
}

void Route::shorten() {
    //
}

CRoute* Route::exportedRoute() {
    auto route = new CRoute[_totalSteps];
    for (pint i = 0; i < _totalSteps; i++) {
        auto index = _list[i];
        route[i].x = index / _column;
        route[i].y = index % _column;
        // This is for python to know when to stop
        if (i == 0)
            route[i].size = _totalSteps;
    }
    return route;
}

pint& Route::operator[](pint index) {
    if (index >= _totalSteps)
        throw std::out_of_range(
            fmt::format("pazusoba::Route index {} out of range {}", index,
                        _totalSteps - 1));

    return _list[index];
}
}  // namespace pazusoba
