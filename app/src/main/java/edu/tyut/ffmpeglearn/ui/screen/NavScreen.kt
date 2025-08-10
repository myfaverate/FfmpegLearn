package edu.tyut.ffmpeglearn.ui.screen

import android.util.Log
import androidx.compose.material3.SnackbarHostState
import androidx.compose.runtime.Composable
import androidx.compose.runtime.CompositionLocalProvider
import androidx.compose.ui.Modifier
import androidx.core.net.toUri
import androidx.lifecycle.viewmodel.compose.LocalViewModelStoreOwner
import androidx.navigation.NavBackStackEntry
import androidx.navigation.NavHostController
import androidx.navigation.compose.NavHost
import androidx.navigation.compose.composable
import androidx.navigation.compose.rememberNavController
import androidx.navigation.toRoute
import edu.tyut.ffmpeglearn.route.Routes

private const val TAG: String = "NavScreen"

@Composable
internal fun NavScreen(
    modifier: Modifier,
    snackBarHostState: SnackbarHostState
){
    val navHostController: NavHostController = rememberNavController()
    CompositionLocalProvider(
        value = LocalViewModelStoreOwner provides checkNotNull(value = LocalViewModelStoreOwner.current) {
            "No ViewModelStoreOwner was provided"
        }
    ) {
        NavHost(
            modifier = modifier,
            navController = navHostController,
            startDestination = Routes.Service
        ) {
            composable<Routes.Greeting>{
                Greeting(
                    name = "Android",
                    navHostController = navHostController,
                    snackBarHostState = snackBarHostState
                )
            }
            composable<Routes.Hello>{
                HelloScreen(
                )
            }
            composable<Routes.Player> { navBackStackEntry:  NavBackStackEntry ->
                val player: Routes.Player = navBackStackEntry.toRoute<Routes.Player>()
                PlayerScreen(
                    videoUri = player.videoUri.toUri()
                )
            }
            composable<Routes.Mp3> { navBackStackEntry:  NavBackStackEntry ->
                Log.i(TAG, "NavScreen -> mp3")
                Mp3Screen(
                    navHostController = navHostController,
                    snackBarHostState = snackBarHostState,
                )
            }
            composable<Routes.Mp4> { navBackStackEntry:  NavBackStackEntry ->
                Log.i(TAG, "NavScreen -> mp4")
                Mp4Screen(
                    navHostController = navHostController,
                    snackBarHostState = snackBarHostState,
                )
            }
            composable<Routes.Surface> { navBackStackEntry:  NavBackStackEntry ->
                Log.i(TAG, "NavScreen -> Surface")
                SurfaceScreen(
                    navHostController = navHostController,
                    snackBarHostState = snackBarHostState,
                )
            }
            composable<Routes.Camera> { navBackStackEntry:  NavBackStackEntry ->
                Log.i(TAG, "NavScreen -> CameraScreen")
                CameraScreen(
                    navHostController = navHostController,
                    snackBarHostState = snackBarHostState,
                )
            }
            composable<Routes.Service> { navBackStackEntry:  NavBackStackEntry ->
                Log.i(TAG, "NavScreen -> ServiceScreen")
                ServiceScreen(
                    navHostController = navHostController,
                    snackBarHostState = snackBarHostState,
                )
            }
        }
    }
}