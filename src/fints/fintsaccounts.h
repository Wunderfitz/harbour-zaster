#ifndef FINTSACCOUNTS_H
#define FINTSACCOUNTS_H

#include <QObject>
#include <QVariantList>

class FinTsAccounts : public QObject
{
    Q_OBJECT
public:
    explicit FinTsAccounts(QObject *parent = 0);

    Q_INVOKABLE QVariantList getAllAccounts();
    Q_INVOKABLE void registerNewAccount();
    Q_INVOKABLE void removeCurrentAccount();
    Q_INVOKABLE void switchAccount(const QString &newAccountUUID);
    Q_INVOKABLE void initializeAccounts();

signals:

public slots:

private:
    QVariantList myAccounts;

};

#endif // FINTSACCOUNTS_H
