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
    Folder *getParentFolder();
    void setParentFolder(Folder *parent);

    virtual PackageTreeItemType getType() = 0;
private:
    Folder *parentFolder;
};

#endif // PACKAGETREEITEM_H
