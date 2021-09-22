import QtQuick 2.9

import "../components" as GyroComponents

TextEdit {
    color: GyroComponents.Style.defaultFontColor
    font.family: GyroComponents.Style.fontRegular.name
    selectionColor: GyroComponents.Style.textSelectionColor
    wrapMode: Text.Wrap
    readOnly: true
    selectByMouse: true
    // Workaround for https://bugreports.qt.io/browse/QTBUG-50587
    onFocusChanged: {
        if(focus === false)
            deselect()
    }
}
