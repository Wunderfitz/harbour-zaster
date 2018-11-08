#include "fintsbalances.h"

FinTsBalances::FinTsBalances(QObject *parent, FinTsDialog *finTsDialog) : QObject(parent)
{
    this->finTsDialog = finTsDialog;
}
