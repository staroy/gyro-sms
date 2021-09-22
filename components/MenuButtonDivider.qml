import QtQuick 2.9

import "." as GyroComponents
import "effects/" as GyroEffects

Rectangle {
    color: GyroComponents.Style.appWindowBorderColor
    height: 1

    GyroEffects.ColorTransition {
        targetObj: parent
        blackColor: GyroComponents.Style._b_appWindowBorderColor
        whiteColor: GyroComponents.Style._w_appWindowBorderColor
    }
}
