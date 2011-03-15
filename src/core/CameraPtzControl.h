#ifndef CAMERAPTZCONTROL_H
#define CAMERAPTZCONTROL_H

#include <QObject>
#include <QMap>
#include "DVRCamera.h"

class QNetworkReply;

class CameraPtzControl : public QObject
{
    Q_OBJECT
    Q_FLAGS(Capability Capabilities)
    Q_FLAGS(Movement Movements)

    Q_PROPERTY(DVRCamera camera READ camera)
    Q_PROPERTY(int protocol READ protocol NOTIFY infoUpdated)
    Q_PROPERTY(Capabilities capabilities READ capabilities NOTIFY infoUpdated)
    Q_PROPERTY(Movements pendingMovements READ pendingMovements NOTIFY pendingMovementsChanged)
    /* May be -1, indicating that we're not currently on a preset (or don't know that we are) */
    Q_PROPERTY(int currentPreset READ currentPreset WRITE moveToPreset NOTIFY currentPresetChanged)
    Q_PROPERTY(QString currentPresetName READ currentPresetName NOTIFY currentPresetChanged)

public:
    enum Capability {
        NoCapabilities = 0,
        CanPan = 1 << 0,
        CanTilt = 1 << 1,
        CanZoom = 1 << 2
    };
    Q_DECLARE_FLAGS(Capabilities, Capability)

    enum Movement {
        NoMovement = 0,
        MoveNorth = 1 << 0,
        MoveSouth = 1 << 1,
        MoveWest = 1 << 2,
        MoveEast = 1 << 3,
        MoveWide = 1 << 4,
        MoveTele = 1 << 5
    };
    Q_DECLARE_FLAGS(Movements, Movement)

    explicit CameraPtzControl(const DVRCamera &camera, QObject *parent = 0);

    const DVRCamera &camera() const { return m_camera; }
    DVRCamera::PtzProtocol protocol() const { return m_protocol; }
    Capabilities capabilities() const { return m_capabilities; }

    bool isReady() const;
    bool hasPendingActions() const;
    Movements pendingMovements() const;

    int currentPreset() const { return m_currentPreset; }
    QString currentPresetName() const { return m_presets.value(m_currentPreset); }

    const QMap<int,QString> &presets() const { return m_presets; }
    int nextPresetID() const;

public slots:
    void move(Movements movements, int panSpeed = -1, int tiltSpeed = -1, int duration = -1);
    void moveToPreset(int preset);
    /* preset ID may be -1 to automatically use the next available ID, which is returned */
    int savePreset(int preset, const QString &name);
    void renamePreset(int preset, const QString &name);
    void clearPreset(int preset);
    void cancel();

signals:
    void infoUpdated();
    void pendingMovementsChanged(Movements pendingMovements);
    void currentPresetChanged(int currentPreset);

private slots:
    void queryResult();
    void moveResult();
    void moveToPresetResult();
    void savePresetResult();
    void clearPresetResult();

private:
    DVRCamera m_camera;
    QMap<int,QString> m_presets;
    DVRCamera::PtzProtocol m_protocol;
    Capabilities m_capabilities;
    int m_currentPreset;

    void sendQuery();
    bool parseResponse(QNetworkReply *reply, QXmlStreamReader &xml, QString &errorMessage);
    bool parsePresetResponse(QXmlStreamReader &xml, int &presetId, QString &presetName);
};

#endif // CAMERAPTZCONTROL_H