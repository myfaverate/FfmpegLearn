package edu.tyut.ffmpeglearn.binder

import android.content.Context
import android.os.Binder
import android.os.IBinder
import android.os.IInterface
import android.os.Parcel
import android.os.Process
import android.util.Log
import edu.tyut.ffmpeglearn.bean.Book
import edu.tyut.ffmpeglearn.utils.Utils

private const val TAG: String = "BookManager2Impl"
private const val DESCRIPTOR: String = "edu.tyut.ffmpeglearn.binder.IBookManager2"
private const val TRANSACTION_getBookList = IBinder.FIRST_CALL_TRANSACTION + 0
private const val TRANSACTION_addBook = IBinder.FIRST_CALL_TRANSACTION + 1

internal class BookManager2Impl internal constructor() : Binder(), IBookManager2 {

    // private val deathRecipient: IBinder.DeathRecipient = IBinder.DeathRecipient { }

    /**
     * 客户端调用
     */
    internal class Proxy(private val iBinder: IBinder) : IBookManager2 {
        override fun getBookList(): List<Book> {
            val data: Parcel = Parcel.obtain()
            val reply: Parcel = Parcel.obtain()
            try {
                data.writeInterfaceToken(DESCRIPTOR)
                val status: Boolean = iBinder.transact(TRANSACTION_getBookList, data, reply, 0)
                Log.i(TAG, "getBookList -> status: $status, pid: ${Process.myPid()}")
                reply.readException()
                val bookList: List<Book> =  reply.createTypedArrayList(Book.CREATOR) ?: emptyList()
                return bookList
            } finally {
                data.recycle()
                reply.recycle()
            }
        }

        override fun addBook(book: Book) {
            val data: Parcel = Parcel.obtain()
            val reply: Parcel = Parcel.obtain()
            try {
                data.writeInterfaceToken(DESCRIPTOR)
                data.writeInt(1)
                book.writeToParcel(data, 0)
                val status: Boolean = iBinder.transact(TRANSACTION_addBook, data, reply, 0)
                Log.i(TAG, "addBook -> status: $status, pid: ${Process.myPid()}")
                reply.readException()
            } finally {
                data.recycle()
                reply.recycle()
            }
        }

        override fun asBinder(): IBinder? {
            return iBinder
        }

    }

    internal companion object {
        internal fun asInterface(iBinder: IBinder): IBookManager2 {
            val queryLocalInterface: IInterface? = iBinder.queryLocalInterface(DESCRIPTOR)
            if (queryLocalInterface != null && queryLocalInterface is IBookManager2){
                return queryLocalInterface
            }
            return BookManager2Impl.Proxy(iBinder)
        }
    }

    init {
        this.attachInterface(this,DESCRIPTOR)
    }

    /**
     * 服务端 binder 线程中 运行
     */
    override fun getBookList(): List<Book> {
        Log.i(TAG, "BookManager2Impl getBookList pid: ${Process.myPid()}")
        return listOf(Book(bookId = 3, bookName = "ServiceImpl数据", isPaper = false))
    }

    /**
     * 服务端 binder 线程中 运行
     */
    override fun addBook(book: Book) {
        Log.i(TAG, "BookManager2Impl addBook -> $book, thread: ${Thread.currentThread()}, pid: ${Process.myPid()}")
    }


    override fun asBinder(): IBinder? {
        return this
    }

    /**
     * 服务端 binder 线程中 运行
     */
    override fun onTransact(code: Int, data: Parcel, reply: Parcel?, flags: Int): Boolean {
        if (code >= FIRST_CALL_TRANSACTION && code <= LAST_CALL_TRANSACTION) {
            data.enforceInterface(DESCRIPTOR)
        }
        return when(code){
            INTERFACE_TRANSACTION -> {
                reply?.writeString(DESCRIPTOR)
                true
            }
            TRANSACTION_getBookList -> {
                val bookList: List<Book> = getBookList()
                reply?.writeNoException()
                reply?.writeTypedList<Book>(bookList)
                Log.i(TAG, "onTransact -> bookList: ${bookList}, thread: ${Thread.currentThread()}, pid: ${Process.myPid()}")
                true
            }
            TRANSACTION_addBook -> {
                val defaultBook: Book = Book(bookId = 0, bookName = "unknown", isPaper = false)
                val book: Book = if(data.readInt() != 0) {
                    Book.CREATOR.createFromParcel(data) ?: defaultBook
                } else {
                    defaultBook
                }
                Log.i(TAG, "onTransact -> book: ${book}, thread: ${Thread.currentThread()}, pid: ${Process.myPid()}")
                this.addBook(book = book)
                reply?.writeNoException()
                true
            }
            else -> {
                super.onTransact(code, data, reply, flags)
            }
        }
    }
}