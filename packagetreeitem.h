#ifndef PACKAGETREEITEM_H
#define PACKAGETREEITEM_H

class Folder;

enum PackageTreeItemType {
    FOLDER,
    PACKAGE
};

class PackageTreeItem
{
public:
    explicit PackageTreeItem(Folder *parentFolder);
    Folder *parentFolder();

    virtual PackageTreeItemType getType() = 0;
private:
    Folder *m_parentFolder;
};

#endif // PACKAGETREEITEM_H
