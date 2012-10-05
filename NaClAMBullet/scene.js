var container, stats;
			var camera, controls, scene, projector, renderer;
			var objects = [], plane;

			var mouse = new THREE.Vector2(),
			offset = new THREE.Vector3(),
			INTERSECTED, SELECTED;

			var sceneDescription = [];
			function init() {

				container = document.createElement( 'div' );
				document.body.appendChild( container );

				camera = new THREE.PerspectiveCamera( 70, window.innerWidth / window.innerHeight, 1, 10000 );
				camera.position.y = 20.0;
				camera.position.z = 40;

				controls = new THREE.TrackballControls( camera );
				controls.rotateSpeed = 1.0;
				controls.zoomSpeed = 5.0;
				controls.panSpeed = 0.8;
				controls.noZoom = false;
				controls.noPan = false;
				controls.staticMoving = true;
				controls.dynamicDampingFactor = 0.3;
				controls.handleResize();

				scene = new THREE.Scene();

				scene.add( new THREE.AmbientLight( 0x505050 ) );

				var light = new THREE.SpotLight( 0xffffff, 1.5 );
				light.position.set( 0, 500, 2000 );
				light.castShadow = true;

				light.shadowCameraNear = 200;
				light.shadowCameraFar = camera.far;
				light.shadowCameraFov = 50;

				light.shadowBias = -0.00022;
				light.shadowDarkness = 0.5;

				light.shadowMapWidth = 2048;
				light.shadowMapHeight = 2048;

				scene.add( light );

				var geometry = new THREE.CubeGeometry( 1, 1, 1 );
				
				for ( var i = 0; i < 100; i ++ ) {

					var object = new THREE.Mesh( geometry, new THREE.MeshLambertMaterial( { color: Math.random() * 0xffffff } ) );

					object.material.ambient = object.material.color;

					object.position.x = Math.random() * 50 - 25;
					object.position.y = Math.random() * 50 + 1;
					object.position.z = Math.random() * 40 - 20;

					object.rotation.x = ( Math.random() * 360 ) * Math.PI / 180;
					object.rotation.y = ( Math.random() * 360 ) * Math.PI / 180;
					object.rotation.z = ( Math.random() * 360 ) * Math.PI / 180;

					object.updateMatrixWorld(true);

					var T = [object.matrixWorld.elements[0],
							 object.matrixWorld.elements[1],
							 object.matrixWorld.elements[2],
							 object.matrixWorld.elements[3],
							 object.matrixWorld.elements[4],
							 object.matrixWorld.elements[5],
							 object.matrixWorld.elements[6],
							 object.matrixWorld.elements[7],
							 object.matrixWorld.elements[8],
							 object.matrixWorld.elements[9],
							 object.matrixWorld.elements[10],
							 object.matrixWorld.elements[11],
							 object.matrixWorld.elements[12],
							 object.matrixWorld.elements[13],
							 object.matrixWorld.elements[14],
							 object.matrixWorld.elements[15]];
					sceneDescription.push(T);
					object.castShadow = true;
					object.receiveShadow = true;
					object.matrixAutoUpdate = false;
					scene.add(object);
					objects.push(object);
				}

				plane = new THREE.Mesh( new THREE.PlaneGeometry( 200, 200, 100, 100), new THREE.MeshBasicMaterial( { color: 0x000000, opacity: 0.25, transparent: true, wireframe: true } ) );
				plane.rotation.x = Math.PI * 0.5;
				plane.visible = true;
				scene.add( plane );
				projector = new THREE.Projector();

				renderer = new THREE.WebGLRenderer( { antialias: true } );
				renderer.sortObjects = false;
				renderer.setSize( window.innerWidth, window.innerHeight );

				renderer.shadowMapEnabled = true;
				renderer.shadowMapSoft = true;

				container.appendChild( renderer.domElement );

				var info = document.createElement( 'div' );
				info.style.position = 'absolute';
				info.style.top = '10px';
				info.style.width = '100%';
				info.style.textAlign = 'center';
				info.innerHTML = '<a href="https://github.com/johnmccutchan/NaClAMBase" target="_blank">NaClAMBase</a> NaClAM - Bullet';
				container.appendChild( info );

				//

				window.addEventListener( 'resize', onWindowResize, false );

			}

			function onWindowResize() {

				camera.aspect = window.innerWidth / window.innerHeight;
				camera.updateProjectionMatrix();

				renderer.setSize( window.innerWidth, window.innerHeight );
				controls.handleResize();
			}


			//

			function animate() {
				requestAnimationFrame(NaClAMBulletStepScene);
				render();
			}

			function render() {
				controls.update();
				renderer.render( scene, camera );
			}