package edu.tyut.ffmpeglearn.binder

import android.os.IInterface
import edu.tyut.ffmpeglearn.bean.Book

internal interface IBookManager2 : IInterface {
    fun getBookList(): List<Book>
    fun addBook(book: Book)
}