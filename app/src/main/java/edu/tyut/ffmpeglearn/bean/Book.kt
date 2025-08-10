package edu.tyut.ffmpeglearn.bean

import android.os.Parcel
import android.os.Parcelable

@ConsistentCopyVisibility
internal data class Book internal constructor(
    private val bookId: Int,
    private val bookName: String,
    // 是否纸质
    private val isPaper: Boolean,
) : Parcelable {

    private constructor(parcel: Parcel?) : this(
        bookId = parcel?.readInt() ?: 0,
        bookName = parcel?.readString() ?: "",
        isPaper = parcel?.readByte() == 1.toByte()
    )

    companion object CREATOR : Parcelable.Creator<Book>{
        override fun createFromParcel(source: Parcel?): Book? {
            return Book(source)
        }

        override fun newArray(size: Int): Array<out Book?>? {
            return arrayOfNulls(size)
        }
    }
    override fun describeContents(): Int {
        return 0
    }

    override fun writeToParcel(dest: Parcel, flags: Int) {
        dest.writeInt(this.bookId)
        dest.writeString(this.bookName)
        dest.writeByte(if(this.isPaper) 1.toByte() else 0.toByte())
    }
}
