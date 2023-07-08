#pragma once

#include <QAbstractListModel>
#include <QObject>

class Zpaces;
class Zpace;
class ZpacesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)

public:

    enum Roles
    {
        ZpaceItem
    };

    explicit ZpacesModel(Zpaces *parent = nullptr);
    ~ZpacesModel();

    int rowCount(const QModelIndex &parent) const override final;
    QVariant data(const QModelIndex &index, int role) const override final;
    QHash<int, QByteArray> roleNames() const override final;

    /**
     * @brief zpace
     * Return the Zpace at the given index, or a nullptr if the index is not valid
     * @param index
     * A valid index
     * @return
     */
    Zpace *zpace(const int &index);

    /**
     * @brief addZpace
     * Adds a new Zpace to the model and returns a pointer to it.
     * @return
     */
    Zpace *addZpace();

    /**
     * @brief insertZpace
     * Inserts a new Zpace at a given index
     * @param index
     * A valid index.
     * @return
     */
    Zpace *insertZpace(const int &index);

    /**
     * @brief removeZpace
     * Removes a Zpace at a given index, this will also delete all its contents, such as surfaces etc.
     * @param index
     * A valid index
     */
    void removeZpace(const int &index);

    /**
     * @brief moveZpace
     * Moves a Zpace from one index to another
     * @param from
     * Initial Zpace index
     * @param to
     * Where to move the Zpace
     */
    void moveZpace(const int &from, const int &to);

    /**
     * @brief zpaces
     * All the current Zpaces avaliable
     * @return
     * List of all the Zpaces AKA QVector<Zpace*>
     */
    QVector<Zpace*> zpaces() const;

    /**
     * @brief indexOf
     * Index of a given Zpace in the model
     * @param space
     * @return
     */
    int indexOf(Zpace *zpace);

    /**
     * @brief zpaceExists
     * Checks if a given Zpace exists in the model
     * @param zpace
     * @return
     */
    bool zpaceExists(Zpace *zpace);

    /**
     * @brief count
     * Total count of the Zpaces in the model
     * @return
     */
    int count() const;

    /**
     * @brief zpacesRoot
     * The Zpaces root object, parent of this model.
     * @return
     */
    Zpaces * zpacesRoot() const;

Q_SIGNALS:
    void countChanged();
    void zpaceAdded(int index);

private:
    QVector<Zpace*> m_zpacesList;
    Zpaces *m_zpacesRoot;
    bool indexIsValid(const int &index) const;
    Zpace * createZpace();
};
