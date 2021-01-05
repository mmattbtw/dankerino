#include "HotkeyModel.hpp"

#include "controllers/hotkeys/Hotkey.hpp"
#include "util/StandardItemHelper.hpp"

namespace chatterino {

HotkeyModel::HotkeyModel(QObject *parent)
    : SignalVectorModel<std::shared_ptr<Hotkey>>(1, parent)
{
}

// turn a vector item into a model row
std::shared_ptr<Hotkey> HotkeyModel::getItemFromRow(
    std::vector<QStandardItem *> &, const std::shared_ptr<Hotkey> &original)
{
    return original;
}

// turns a row in the model into a vector item
void HotkeyModel::getRowFromItem(const std::shared_ptr<Hotkey> &item,
                                 std::vector<QStandardItem *> &row)
{
    setStringItem(row[0], item->toString(), false);
    row[0]->setData(QFont("Segoe UI", 10), Qt::FontRole);
}

int HotkeyModel::beforeInsert(const std::shared_ptr<Hotkey> &item,
                              std::vector<QStandardItem *> &row,
                              int proposedIndex)
{
    if (this->categoryCount_[item->getCategory()]++ == 0)
    {
        auto newRow = this->createRow();

        setStringItem(newRow[0], item->getCategory(), false, false);
        newRow[0]->setData(QFont("Segoe UI Light", 16), Qt::FontRole);

        this->insertCustomRow(std::move(newRow), proposedIndex);

        return proposedIndex + 1;
    }

    return proposedIndex;
}

void HotkeyModel::afterRemoved(const std::shared_ptr<Hotkey> &item,
                               std::vector<QStandardItem *> &row, int index)
{
    auto it = this->categoryCount_.find(item->getCategory());
    assert(it != this->categoryCount_.end());

    if (it->second <= 1)
    {
        this->categoryCount_.erase(it);
        this->removeCustomRow(index - 1);
    }
    else
    {
        it->second--;
    }
}

}  // namespace chatterino
