/*
 * Copyright (c) 2011-2015 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import bb.cascades 1.4

Page {
    titleBar: TitleBar {
        title: "Search Giphy"
    }
    
    Container {
        topPadding: 20
        bottomPadding: 20
        rightPadding: 20
        leftPadding: 20
        
        /*
        Label {
            // Localized text with the dynamic translation and locale updates support
            text: qsTr("Hello World") + Retranslate.onLocaleOrLanguageChanged
            textStyle.base: SystemDefaults.TextStyles.BigText
        }
        */
        
        TextField {
            objectName: "searchField"
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Top
            bottomMargin: 20
            
            hintText: "Enter Search Term"
            inputMode: TextFieldInputMode.Default
            textStyle.base: SystemDefaults.TextStyles.BodyText
            
            input {
                submitKey: SubmitKey.Search
            }
        }
        
        ImageView {
             objectName: "theImage"
             horizontalAlignment: HorizontalAlignment.Center
             verticalAlignment: VerticalAlignment.Fill
        }
          
        ActivityIndicator {
            objectName: "loading"
            horizontalAlignment: HorizontalAlignment.Fill
            preferredWidth: 300
            topMargin: 20
        }   
    }
}
