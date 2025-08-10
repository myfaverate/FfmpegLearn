package edu.tyut.ffmpeglearn.service

import android.content.Context
import android.content.Intent
import android.os.IBinder
import android.os.Process
import android.util.Log
import androidx.lifecycle.LifecycleService
import edu.tyut.ffmpeglearn.IBookManager
import edu.tyut.ffmpeglearn.bean.Book
import edu.tyut.ffmpeglearn.binder.BookManager2Impl
import edu.tyut.ffmpeglearn.utils.Utils

private const val TAG: String = "HelloService"

internal class HelloService internal constructor() : LifecycleService() {

    internal companion object {
        internal fun startService(context: Context){
            context.startService(Intent(context, HelloService::class.java))
        }
    }

    // private val binder: IBookManager.Stub = object : IBookManager.Stub() {
    //     override fun getBookList(): List<Book?>? {
    //         Log.i(TAG, "getBookList -> pid: ${Process.myPid()}")
    //         return listOf(Book(bookId = 1, bookName = "红楼梦", isPaper = false))
    //     }
    //
    //     override fun addBook(book: Book?) {
    //         Log.i(TAG, "addBook -> book: $book, thread: ${Thread.currentThread()}")
    //     }
    // }

    private val binder: BookManager2Impl = BookManager2Impl()

    override fun onBind(intent: Intent): IBinder? {
        super.onBind(intent)
        Log.i(TAG, "onBind -> pid: ${Process.myPid()}, processName: ${Utils.getProcessName(this)}")
        return binder
    }
}