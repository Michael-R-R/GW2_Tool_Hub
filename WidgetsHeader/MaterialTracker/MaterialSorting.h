#ifndef MATERIALSORTING_H
#define MATERIALSORTING_H

#include "WidgetsHeader/MaterialTracker/Materials.h"
#include <algorithm>
#include <QVector>
#include <QDebug>

class MaterialSorting
{
public:
    MaterialSorting();
    ~MaterialSorting();

    // Category
    QVector<Materials*> SortLowToHighCategory(QVector<Materials*>& materials);
    QVector<Materials*> SortHighToLowCategory(QVector<Materials*>& materials);

    // Name
    QVector<Materials*> SortLowToHighName(QVector<Materials*>& materials);
    QVector<Materials*> SortHighToLowName(QVector<Materials*>& materials);

    // Current
    QVector<Materials*> SortLowToHighCurrent(QVector<Materials*>& materials);
    QVector<Materials*> SortHighToLowCurrent(QVector<Materials*>& materials);

    // Goal
    QVector<Materials*> SortLowToHighGoal(QVector<Materials*>& materials);
    QVector<Materials*> SortHighToLowGoal(QVector<Materials*>& materials);

    // Percent
    QVector<Materials*> SortLowToHighPercent(QVector<Materials*>& materials);
    QVector<Materials*> SortHighToLowPercent(QVector<Materials*>& materials);

private:

    // Compare category
    static bool CompareLessThanCategory(const Materials* lhs, const Materials* rhs);
    static bool CompareGreaterThanCategory(const Materials* lhs, const Materials* rhs);

    // Compare name
    static bool CompareLessThanName(const Materials* lhs, const Materials* rhs);
    static bool CompareGreaterThanName(const Materials* lhs, const Materials* rhs);

    // Compare current
    static bool CompareLessThanCurrent(const Materials* lhs, const Materials* rhs);
    static bool CompareGreaterThanCurrent(const Materials* lhs, const Materials* rhs);

    // Compare goal
    static bool CompareLessThanGoal(const Materials* lhs, const Materials* rhs);
    static bool CompareGreaterThanGoal(const Materials* lhs, const Materials* rhs);

    // Compare percents
    static bool CompareLessThanPercentages(const Materials* lhs, const Materials* rhs);
    static bool CompareGreaterThanPercentages(const Materials* lhs, const Materials* rhs);

};

#endif // MATERIALSORTING_H
