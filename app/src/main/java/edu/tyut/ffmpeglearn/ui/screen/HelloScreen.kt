package edu.tyut.ffmpeglearn.ui.screen

import androidx.compose.foundation.layout.Column
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.lifecycle.ViewModelStoreOwner
import androidx.lifecycle.viewmodel.compose.LocalViewModelStoreOwner

private const val TAG: String = "HelloScreen"

@Composable
internal fun HelloScreen(){
    Column(
        modifier = Modifier
    ) {
        Text(text = "Hello")
    }
}