package edu.tyut.ffmpeglearn.ui.screen

import android.app.Service
import android.content.ComponentName
import android.content.Context
import android.content.Intent
import android.content.ServiceConnection
import android.os.IBinder
import android.os.Process
import android.util.Log
import androidx.activity.contextaware.ContextAware
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.SnackbarHostState
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.unit.dp
import androidx.lifecycle.ViewModelStoreOwner
import androidx.lifecycle.viewmodel.compose.LocalViewModelStoreOwner
import androidx.navigation.NavHostController
import edu.tyut.ffmpeglearn.IBookManager
import edu.tyut.ffmpeglearn.bean.Book
import edu.tyut.ffmpeglearn.binder.BookManager2Impl
import edu.tyut.ffmpeglearn.binder.IBookManager2
import edu.tyut.ffmpeglearn.service.HelloService
import edu.tyut.ffmpeglearn.ui.theme.RoundedCornerShape10
import edu.tyut.ffmpeglearn.utils.Utils
import kotlinx.coroutines.launch

private const val TAG: String = "ServiceScreen"

@Composable
internal fun ServiceScreen(
    navHostController: NavHostController,
    snackBarHostState: SnackbarHostState,
){
    val context: Context = LocalContext.current
    // var bookManager: IBookManager? by remember {
    //     mutableStateOf(value = null)
    // }
    // val connection by remember {
    //     mutableStateOf(value = object : ServiceConnection {
    //         override fun onServiceConnected(
    //             name: ComponentName?,
    //             service: IBinder?
    //         ) {
    //             val asInterface: IBookManager? = IBookManager.Stub.asInterface(service)
    //             bookManager = asInterface
    //         }
    //
    //         override fun onServiceDisconnected(name: ComponentName?) {
    //             Log.i(TAG, "onServiceDisconnected -> name: $name")
    //         }
    //     })
    // }
    var bookManager: IBookManager2? by remember {
        mutableStateOf(value = null)
    }
    val connection by remember {
        mutableStateOf(value = object : ServiceConnection {
            override fun onServiceConnected(
                name: ComponentName?,
                service: IBinder?
            ) {
                bookManager = BookManager2Impl.asInterface(service!!)
            }

            override fun onServiceDisconnected(name: ComponentName?) {
                Log.i(TAG, "onServiceDisconnected -> name: $name")
            }
        })
    }
    Column(
        modifier = Modifier
    ) {
        Text(
            text = "启动服务",
            Modifier
                .padding(top = 10.dp)
                .background(color = Color.Black, shape = RoundedCornerShape10)
                .padding(all = 5.dp)
                .clickable {
                    Log.i(TAG, "ServiceScreen pid: ${Process.myPid()} getProcessName: ${Utils.getProcessName(context)}")
                    context.bindService(
                        Intent(context, HelloService::class.java),
                        connection,
                        Service.BIND_AUTO_CREATE
                    )
                },
            color = Color.White
        )
        Text(
            text = "远程调用",
            Modifier
                .padding(top = 10.dp)
                .background(color = Color.Black, shape = RoundedCornerShape10)
                .padding(all = 5.dp)
                .clickable {
                    bookManager?.addBook(Book(bookId = 2, bookName = "洗衣机", isPaper = false))
                    Log.i(TAG, "ServiceScreen -> bookList: ${bookManager?.getBookList()}")
                    // Log.i(TAG, "ServiceScreen -> bookList: ${bookManager?.bookList}")
                },
            color = Color.White
        )
    }
}