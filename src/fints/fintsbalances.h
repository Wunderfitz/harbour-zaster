#ifndef FINTSBALANCES_H
#define FINTSBALANCES_H

#include <QObject>
#include <QVariantList>
#include "fintsdialog.h"
#include "updconstants.h"

class FinTsBalances : public QObject
{
    Q_OBJECT
public:
    explicit FinTsBalances(QObject *parent = 0, FinTsDialog *finTsDialog = 0);
    Q_INVOKABLE void retrieveBalances();

signals:
    void balancesRetrieved(const QVariantList &accountBalances);
    void errorRetrievingBalances(const QString &errorMessage);

public slots:

private slots:
    void handleDialogInitializationCompleted();
    void handleDialogInitializationFailed();
    void handleAccountBalanceCompleted(const QVariantList &accountBalances);
    void handleAccountBalanceFailed();
    void handleDialogEndCompleted();
    void handleDialogEndFailed();
    void handleErrorOccurred();

private:
    FinTsDialog *finTsDialog;
    bool workInProgress;
    bool shallHandleDialogEnd;
    bool inError;
    QString accountInProgress;
    QVariantList myAccounts;
    QVariantList retrievedAccounts;
    QVariantList retrievedBalances;

    void setWorkInProgress(const bool &inProgress);
    void handleErrorStatus();
};

#endif // FINTSBALANCES_H
