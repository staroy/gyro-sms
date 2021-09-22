import QtQuick 2.9

import "." as GyroComponents
import "effects/" as GyroEffects

Text {
    // When using this component, please note that if you use a color different
    // than `defaultFontColor`, you are required to also define `themeTransitionXColor`.
    // If you do not set these the component will receive the wrong color after a transition.
    // If you do not want to set these, use `themeTransition: false`.
    id: root
    property bool themeTransition: true
    property string themeTransitionBlackColor: ""
    property string themeTransitionWhiteColor: ""
    font.family: GyroComponents.Style.fontMedium.name
    font.bold: false
    font.pixelSize: 14
    textFormat: Text.PlainText

    GyroEffects.ColorTransition {
        enabled: root.themeTransition
        themeTransition: root.themeTransition
        targetObj: root
        duration: 750
        blackColor: root.themeTransitionBlackColor !== "" ? root.themeTransitionBlackColor : GyroComponents.Style._b_defaultFontColor
        whiteColor: root.themeTransitionWhiteColor !== "" ? root.themeTransitionWhiteColor : GyroComponents.Style._w_defaultFontColor
    }
}
