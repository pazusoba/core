#ifndef Q_BRIDGE_H
#define Q_BRIDGE_H

#include <QObject>
#include <QVector>
#include "../../core/solver.h"
#include "michi.h"

class QBridge : public QObject
{
    Q_OBJECT

    // Properties
    Q_PROPERTY(int row MEMBER m_row NOTIFY rowChanged)
    Q_PROPERTY(int column MEMBER m_column NOTIFY columnChanged)
    Q_PROPERTY(std::vector<int> initialBoard MEMBER m_initialBoard NOTIFY initialBoardChanged)
    Q_PROPERTY(QList<QRoute *> routes MEMBER m_routes NOTIFY routeChanged)

    // Constants
    Q_PROPERTY(QStringList stepList MEMBER m_stepList CONSTANT)
    Q_PROPERTY(QStringList beamList MEMBER m_beamList CONSTANT)

public:
    explicit QBridge(QObject *parent = nullptr);

signals:
    void rowChanged();
    void columnChanged();
    void routeChanged();
    void initialBoardChanged();

public slots:
    void solve();
    void random();
    void updateBeam(int index);
    void updateStep(int index);

private:
    // Define the size of the board
    int m_row, m_column;
    // This is the initial board for display in the beginning
    std::vector<int> m_initialBoard;
    // All routes
    QList<QRoute *> m_routes;

    // Let's just pass in some default parameter first
    PSolver solver;

    // Other constants for the GUI
    bool shouldSolve = true;
    const QStringList m_stepList = { "20", "25", "30", "40", "50" };
    const QStringList m_beamList = { "100", "500", "1000", "5000", "10000" };
};

#endif // Q_BRIDGE_H
