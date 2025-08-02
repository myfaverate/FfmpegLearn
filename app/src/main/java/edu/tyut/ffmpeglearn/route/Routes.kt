package edu.tyut.ffmpeglearn.route

import kotlinx.serialization.Serializable

internal sealed class Routes {
    @Serializable
    internal object Greeting
    @Serializable
    internal object Hello
    @Serializable
    internal object Mp3
    @Serializable
    internal object Mp4
    @Serializable
    internal object Surface
    @Serializable
    internal data class Player(
        val videoUri: String,
    )
}