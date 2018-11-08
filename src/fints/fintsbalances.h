#ifndef FINTSBALANCES_H
#define FINTSBALANCES_H

#include <QObject>
#include "fintsdialog.h"

class FinTsBalances : public QObject
{
    Q_OBJECT
public:
    explicit FinTsBalances(QObject *parent = 0, FinTsDialog *finTsDialog = 0);

signals:

public slots:

private:
    FinTsDialog *finTsDialog;
};

#endif // FINTSBALANCES_H
