document.addEventListener('DOMContentLoaded', function () {
	const temperatureValue = document.getElementById("temperature");
	const humidityValue = document.getElementById("humidity");
	const rainValue = document.getElementById("rainfall");
	const lightValue = document.getElementById("brightness");

	const temperatureImage = document.getElementById("temperature-image");
	const humidityImage = document.getElementById("humidity-image");
	const rainImage = document.getElementById("rain-image");
	const lightImage = document.getElementById("light-image");

	const weatherElement = document.getElementById('change-state');

	// Replace with your actual Firebase configuration
	var firebaseConfig = {
		// ... your config
	};

	// Initialize Firebase
	firebase.initializeApp(firebaseConfig);
	firebase.analytics();

	var temperatureRef = firebase.database().ref().child('temp');
	temperatureRef.on('value', function (snapshot) {
		const temperatureData = snapshot.val();
		temperatureValue.textContent = temperatureData + " °C";

		// Sử dụng đường dẫn tương đối đến thư mục img 
		if (temperatureData < 10) {
			temperatureImage.src = "img/temperature-cold.png"; 
		} else if (temperatureData > 30) {
			temperatureImage.src = "img/temperature-hot.png"; 
		} else {
			temperatureImage.src = "img/temperature.png"; 
		}
	});

	var humidityRef = firebase.database().ref().child('humi');
	humidityRef.on('value', function (snapshot) {
		const humidityData = snapshot.val();
		humidityValue.textContent = humidityData + " %";

		if (humidityData < 30) {
			humidityImage.src = "img/humidity-low.png"; 
		} else if (humidityData > 70) {
			humidityImage.src = "img/humidity-high.png"; 
		} else {
			humidityImage.src = "img/humidity.png"; 
		}
	});

	var rainRef = firebase.database().ref().child('rain');
	rainRef.on('value', function (snapshot) {
		const rainData = snapshot.val();
		rainValue.textContent = rainData;

		if (rainData > 0) {
			weatherElement.textContent = "Rain";
			rainImage.src = "img/rain-yes.jpg"; 
		} else {
			weatherElement.textContent = "No Rain";
			rainImage.src = "img/rain-no.png"; 
		}
	});

	var lightRef = firebase.database().ref().child('light');
	lightRef.on('value', function (snapshot) {
		const lightData = snapshot.val();
		lightValue.textContent = lightData + " Lux";

		if (lightData < 100) {
			lightImage.src = "img/light-dark.png"; 
		} else if (lightData > 1000) {
			lightImage.src = "img/light-bright.png"; 
		} else {
			lightImage.src = "img/light.png"; 
		}
	});
});
