#include "WidgetsHeader/MaterialTracker/MaterialSorting.h"

MaterialSorting::MaterialSorting()
{

}

MaterialSorting::~MaterialSorting()
{

}

/*************************************************************************
 *                         SORTING FUNCTIONS                             *
 *************************************************************************/

QVector<Materials*> MaterialSorting::SortLowToHighCategory(QVector<Materials *>& materials)
{
    std::sort(materials.begin(), materials.end(), &MaterialSorting::CompareLessThanCategory);

    return materials;
}

QVector<Materials*> MaterialSorting::SortHighToLowCategory(QVector<Materials *>& materials)
{
    std::sort(materials.begin(), materials.end(), &MaterialSorting::CompareGreaterThanCategory);

    return materials;
}

QVector<Materials*> MaterialSorting::SortLowToHighName(QVector<Materials *>& materials)
{
    std::sort(materials.begin(), materials.end(), &MaterialSorting::CompareLessThanName);

    return materials;
}

QVector<Materials*> MaterialSorting::SortHighToLowName(QVector<Materials *>& materials)
{
    std::sort(materials.begin(), materials.end(), &MaterialSorting::CompareGreaterThanName);

    return materials;
}

QVector<Materials *> MaterialSorting::SortLowToHighCurrent(QVector<Materials *> &materials)
{
    std::sort(materials.begin(), materials.end(), &MaterialSorting::CompareLessThanCurrent);

    return materials;
}

QVector<Materials *> MaterialSorting::SortHighToLowCurrent(QVector<Materials *> &materials)
{
    std::sort(materials.begin(), materials.end(), &MaterialSorting::CompareGreaterThanCurrent);

    return materials;
}

QVector<Materials *> MaterialSorting::SortLowToHighGoal(QVector<Materials *> &materials)
{
    std::sort(materials.begin(), materials.end(), &MaterialSorting::CompareLessThanGoal);

    return materials;
}

QVector<Materials *> MaterialSorting::SortHighToLowGoal(QVector<Materials *> &materials)
{
    std::sort(materials.begin(), materials.end(), &MaterialSorting::CompareGreaterThanGoal);

    return materials;
}

QVector<Materials*> MaterialSorting::SortLowToHighPercent(QVector<Materials*>& materials)
{
    std::sort(materials.begin(), materials.end(), &MaterialSorting::CompareLessThanPercentages);

    return materials;
}

QVector<Materials*> MaterialSorting::SortHighToLowPercent(QVector<Materials *>& materials)
{
    std::sort(materials.begin(), materials.end(), &MaterialSorting::CompareGreaterThanPercentages);

    return materials;
}

/*************************************************************************
 *                           COMPARATORS                                 *
 *************************************************************************/

bool MaterialSorting::CompareLessThanCategory(const Materials *lhs, const Materials *rhs)
{
   return (lhs->GetMaterialCategory() < rhs->GetMaterialCategory());
}

bool MaterialSorting::CompareGreaterThanCategory(const Materials *lhs, const Materials *rhs)
{
    return (lhs->GetMaterialCategory() > rhs->GetMaterialCategory());
}

bool MaterialSorting::CompareLessThanName(const Materials *lhs, const Materials *rhs)
{
    return (lhs->GetMaterialName() < rhs->GetMaterialName());
}

bool MaterialSorting::CompareGreaterThanName(const Materials *lhs, const Materials *rhs)
{
    return (lhs->GetMaterialName() > rhs->GetMaterialName());
}

bool MaterialSorting::CompareLessThanCurrent(const Materials *lhs, const Materials *rhs)
{
    return (lhs->GetMaterialCurrentAmt() < rhs->GetMaterialCurrentAmt());
}

bool MaterialSorting::CompareGreaterThanCurrent(const Materials *lhs, const Materials *rhs)
{
    return (lhs->GetMaterialCurrentAmt() > rhs->GetMaterialCurrentAmt());
}

bool MaterialSorting::CompareLessThanGoal(const Materials *lhs, const Materials *rhs)
{
    return (lhs->GetMaterialGoalAmt() < rhs->GetMaterialGoalAmt());
}

bool MaterialSorting::CompareGreaterThanGoal(const Materials *lhs, const Materials *rhs)
{
    return (lhs->GetMaterialGoalAmt() > rhs->GetMaterialGoalAmt());
}

// Sort by material percentages from lowest to highest
bool MaterialSorting::CompareLessThanPercentages(const Materials* lhs, const Materials* rhs)
{
    return (lhs->GetMaterialPercentComplete() < rhs->GetMaterialPercentComplete());
}

// Sort by material percentages from highest to lowest
bool MaterialSorting::CompareGreaterThanPercentages(const Materials *lhs, const Materials *rhs)
{
    return (lhs->GetMaterialPercentComplete() > rhs->GetMaterialPercentComplete());
}
